--Firt GameMode file !
AddCSLuaFile("GameModes/Sandbox/cl_init.lua")

function GM:EntitySpawned(ent)
	--print(ent)
end

function EntitySpawned(ent)
	print(ent)
	print(ent:GetPos())
end

function GM:Initialize(test, str)
	--print(str)
	--print(test)
	--self:DoSomething(str)
end

function GM:DoSomething(str)
	--print(str .. " : test")
end

function GM:DrawHUD()
end