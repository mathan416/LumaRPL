local ok,message=pcall(function() screen.font("tiny") end)
print(ok,type(message))
print(screen.font())
print(screen.width,screen.height)
