function GM:Initialize(test, str)
	print(str)
	print(test)
	self:DoSomething(str)
end

function GM:DoSomething(str)
	print(str .. " : test")
end