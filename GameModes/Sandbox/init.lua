--Firt GameMode file !
AddCSLuaFile("GameModes/Sandbox/cl_init.lua")
Include("Addons/test.lua")
GlobalTest = nil

function GM:EntitySpawned(ent)
	print(ent)
end

--Stack trace system test
local function DoSpawn2()
	SpawnEntity()
end
local function DoSpawn1()
	DoSpawn2()
end
local function DoSpawn0()
	DoSpawn1()
end

local flag = false
function EntitySpawned(ent)
	--if (ent == GlobalTest) then
	--	GlobalTest:SetModel("UMod:Cup")
	--	print("test")
	--end
	--print(ent)
	--print(ent:GetPos())
	--print(ent:GetClass())
	if (not(flag)) then
		--DoSpawn0()
		GlobalTest = ents.Create("PhysicsProp")
		GlobalTest:SetPos(Vector(0, 0, 1500))		
		GlobalTest:SetModel("UMod:Cup")
		--print("SetModel called LuaSide")
		--print(GlobalTest:EntIndex())
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