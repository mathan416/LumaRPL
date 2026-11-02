#pragma once
// PicoCalc presentation adapter: explicit base literals keep their value while
// Bin/Oct/Dec/Hex also change the selected stack value's display base.
inline bool pico_prepare_base_display(object::id command)
{
 if(command!=object::ID_Bin&&command!=object::ID_Oct&&command!=object::ID_Dec&&command!=object::ID_Hex)return true;
 if(!rt.depth())return true;object_g top=rt.top();if(!top)return true;object::id target;
 switch(top->type()){
 case object::ID_integer:case object::ID_bignum:
 case object::ID_neg_integer:case object::ID_neg_bignum:
  return RealToBinary::evaluate()==object::OK;
 case object::ID_bin_integer:case object::ID_oct_integer:case object::ID_dec_integer:case object::ID_hex_integer:target=object::ID_based_integer;break;
 case object::ID_bin_bignum:case object::ID_oct_bignum:case object::ID_dec_bignum:case object::ID_hex_bignum:target=object::ID_based_bignum;break;
 default:return true;
 }
 ASSERT(leb128size(top->type())==leb128size(target));
 object_g converted=rt.clone(top);if(!converted)return false;byte *data=(byte *)+converted;leb128(data,target);return rt.top(converted);
}
