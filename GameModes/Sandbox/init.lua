--Firt GameMode file !
AddCSLuaFile("GameModes/Sandbox/cl_init.lua")

GlobalTest = nil

function GM:EntitySpawned(ent)
	--print(ent)
end

local flag = false
function EntitySpawned(ent)
	--print(ent)
	--print(ent:GetPos())
	--print(ent:GetClass())
	if (not(flag)) then
		GlobalTest = ents.Create("PhysicsProp")
		GlobalTest:SetPos(Vector(0, 0, 1500))
		--GlobalTest:SetModel("Cup")
		print(GlobalTest:EntIndex())
		flag = true
	end
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