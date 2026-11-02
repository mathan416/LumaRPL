"""Execute the assembled PIO instructions with bounded, undrained 4-word FIFOs."""
import re,unittest
from collections import deque
from pathlib import Path
root=Path(__file__).resolve().parents[1]
class PsramPio(unittest.TestCase):
    def test_all_transaction_lengths(self):
        header=(root/'build/pico/psram_spi.pio.h').read_text()
        body=header.split('picocalc_psram_program_instructions[] = {',1)[1].split('};',1)[0]
        program=[int(n,16) for n in re.findall(r'0x([a-f0-9]{4}),',body)]
        idle=int(re.search(r'picocalc_psram_offset_idle (\d+)',header)[1])
        maximum=0
        for length in range(1,14):
            tx=bytes((i*73+19)&255 for i in range(length));expected=bytes((i*97+43)&255 for i in range(length))
            fifo=deque(int.from_bytes(tx[i:i+4].ljust(4,b'\0'),'big') for i in range(0,length,4));self.assertLessEqual(len(fifo),4)
            rx=[];x=length-1;y=0;pc=0;osr=0;out_count=32;isr=0;in_count=0;bits=0;mosi=[];low=0;steps=0
            while pc!=idle:
                self.assertLess(steps,400);steps+=1
                instruction=program[pc];side=(instruction>>11)&3;opcode=instruction>>13;arg=instruction&255;next_pc=pc+1
                if not side&1:low+=1+((instruction>>8)&7)
                if opcode==7:
                    self.assertEqual(side,0);self.assertEqual(arg>>5,2);y=arg&31
                elif opcode==3:
                    self.assertEqual(side,0);self.assertEqual(arg,1)
                    if out_count==32:self.assertTrue(fifo);osr=fifo.popleft();out_count=0
                    mosi.append(osr>>31);osr=(osr<<1)&0xffffffff;out_count+=1
                elif opcode==2:
                    self.assertEqual(side,2);self.assertEqual(arg,1);self.assertLess(bits,length*8)
                    isr=((isr<<1)|((expected[bits//8]>>(7-bits%8))&1))&0xffffffff;bits+=1;in_count+=1
                    if in_count==32:self.assertLess(len(rx),4);rx.append(isr);isr=in_count=0
                elif opcode==0:
                    self.assertEqual(side,0);condition=arg>>5;target=arg&31
                    if condition==4:
                        if y:next_pc=target
                        y=(y-1)&0xffffffff
                    elif condition==2:
                        if x:next_pc=target
                        x=(x-1)&0xffffffff
                    else:self.fail('unexpected jump')
                elif opcode==5:
                    self.assertEqual(side,0);self.assertEqual(arg,0x42) # NOP is MOV Y,Y.
                elif opcode==4:
                    self.assertEqual(side,1);self.assertEqual(arg,0);self.assertLess(len(rx),4);rx.append(isr);isr=in_count=0
                else:self.fail('unexpected opcode')
                pc=next_pc
            decoded=bytearray()
            for i in range(0,length,4):
                count=min(4,length-i);decoded.extend(rx[i//4].to_bytes(4,'big')[-count:])
            self.assertEqual(decoded,expected)
            self.assertEqual(bytes(sum(mosi[i+j]<<(7-j) for j in range(8)) for i in range(0,len(mosi),8)),tx)
            self.assertEqual(bits,length*8);self.assertLessEqual(len(rx),4)
            self.assertEqual(side,1);self.assertLess(low/60,6);maximum=max(maximum,low)
        print(f'PASS: assembled PIO, all 1–13 byte full-duplex transfers, MSB ordering, partial words, undrained FIFOs and CS low <= {maximum/60:.3f} us at configured 60 MHz')
if __name__=='__main__':unittest.main()
