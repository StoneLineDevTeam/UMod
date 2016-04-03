--THE VERY VERY FIRST BEGINING OF LUA MESS !
--This file is just here to mess a bit with debug LuaEngine

AddCSLuaFile("UMod_CL.lua") --Don't AddCSLuaFile or enjoy client crashes (I tried to files in one packet, that is obviously the best way to crash UE4... I'll come back with file send part by part.)
AddCSLuaFile("UMod_TestSendFile.txt")

print("this is a test !") --Print function is overwritten by C++ code as it needs to redirect to UE_LOG so we can display in "Game Console" window.
log.Error("Log lib test !") --log.* library Working 100%

function GM:Initialize(str) --"str" is always a nil value and I don't know why @see UUModGameInstance.cpp in function Init @see LuaEngine::RunScriptFunction
	print(str) --This time it should finaly work, well maybe not...
	local tbl = game.GetMapList() --Working
	print(#tbl) --UE_LOG 3 (Normal)
	print(type(tbl))
	for k, v in pairs(tbl) do
		log.Warning("Map : Name=" .. v.NiceName .. "|Path=" .. v.Path .. "|Category=" .. v.Category)
	end
	
	print(AssetType.MODEL) --UE_LOG 2 (Normal)
	local tbl1 = game.GetAssetList(AssetType.MODEL) --Error "attempt to index a number value" (Abnormal/Disfunctional method and by extention strange error)
	print(#tbl1)
	for k, v in pairs(tbl1) do
		log.Warning("NewModel : " .. v.Path)
	end
	
	--Now it's time to crash the game !
	--More infos : when calling UUModGameInstance->Disconnect(FString) when no connection are established or if running dedicated server then game instantly crash as it tries to kill NULL session...
	--game.Disconnect("ENJOY ACCESS VIOLATION !")
	
	--Yeah before you say it I'm not crazy or maybe a bit...
	--And yeah that's not the best function I ever added...
	--game.ShowFatalMessage("Yuri6037 and Lua are having fun time !!")
	
	local tbl = Color(255, 0, 0, 255) --Test : the Color function should return a table with r, g, b and a values
	print(tbl.R)
	print(tbl.G)
	print(tbl.B)
	print(tbl.A)
	
	print(0)
	print(true)
	print(nil)
	
	local t = 52
	print(type(t))
end


--Big test, just to know if URender2D bridge works (Server is able to load a texture !! Server is CRAZY !)
--First real scrip part : render some squares on the HUD !
local id = surface.LoadTexture("Internal/Textures/FirstPersonCrosshair")
function GM:DrawHUD()
	surface.ResetTexture()
	local col = Color(255, 255, 255, 128)
	print(col.R .. "," .. col.G .. "," .. col.B .. "," .. col.A)
	surface.SetColor(col)
	surface.DrawRect(0, 0, 1300, 720)
end