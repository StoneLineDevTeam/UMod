--Firt GameMode file (Client) !

local fntIndex = surface.LoadFont("UMod:FederationRegular", 32)
local GVersion = game.GetVersion()
ScrW = 0
ScrH = 0

function GM:DrawHUD()
	surface.SetColor(255, 255, 255)
	surface.SetFontScale(0.5, 0.5)
	surface.DrawText("UMod " .. GVersion, ScrW / 2, ScrH - 64, DrawEnums.TEXT_ALIGN_CENTER) --Just as test puposes
end

function GM:Initialize()
	log.Info("Sandbox GameMode initializing !")
	ScrW, ScrH = ScrSize()
end