--[[
!!IMPORTANT!!

When UI is finalized, add a local script that preloads the title screen PRIOR to loading in the main asset group.

]]

-- Services
local ContentService = game:GetService("ContentProvider");
local TweenService = game:GetService("TweenService");
local RunService = game:GetService("RunService");
local ReplicatedStorage = game:GetService("ReplicatedStorage");
local StarterGui = game:GetService("StarterGui");
local UIS = game:GetService("UserInputService");
local Lighting = game:GetService("Lighting");

-- Assets
local ScreenGUI = script.Parent;
local Song = script:WaitForChild("music");
local MainFrame = ScreenGUI:WaitForChild("Frame");
local Camera = workspace.CurrentCamera;
local TitlescreenScene = workspace:WaitForChild("TitleScreen");
local TitlescreenSceneCamera = TitlescreenScene:WaitForChild("LoadScreen");
local PlayscreenSceneCamera = TitlescreenScene:WaitForChild("PlayScene");
local SpawnEvent = ReplicatedStorage:WaitForChild("TempSpawnEvent");
local NormalDOF = Lighting:WaitForChild("DepthOfField");
local SceneDOF = Lighting:WaitForChild("TitleScreenDepth");

-- Main Frame Assets
local LoadingBarCanvas = MainFrame:WaitForChild("LoadingBarCanvas");
local PlayButtonCanvas = MainFrame:WaitForChild("PlayButtonCanvas");
local UpdateLogCanvas = MainFrame:WaitForChild("UpdateLogCanvas");
local LoadingBar = LoadingBarCanvas:WaitForChild("Loading Bar");
local TipBar = MainFrame:WaitForChild("Tips Frame");
local Tips = {
	[1] = MainFrame:WaitForChild("Tip 1"),
	[2] = MainFrame:WaitForChild("Tip 2"),
	[3] = MainFrame:WaitForChild("Tip 3"),
};

-- Constants
local MaxCameraOffset = 3;
local TipAmount = 0;
local TipChangeSpeed = 8; -- Tips change every 10 seconds
local LoadingBarTweenInfo__PercentChanged = TweenInfo.new(0.15, Enum.EasingStyle.Sine);
local LoadingBarTweenInfo__Completed = TweenInfo.new(0.75, Enum.EasingStyle.Sine);
local TipChangeTweenInfo = TweenInfo.new(0.4, Enum.EasingStyle.Sine);
local SceenChangeTweenInfo = TweenInfo.new(0.25, Enum.EasingStyle.Sine);
local SceenChangeDelay = 0.5;
local OriginPlaySize = PlayButtonCanvas.Size;
local CameraSwaySpeed = 0.3;
local PlayButtonScalarOffset = 0.025;
local PlayButtonScalarSpeed = 1.9;

for _, v in pairs(Tips) do
	TipAmount += 1;
end

-- Variable
local assetsToLoad = 0;
local assetsLoaded = 0;
local active = false;
local loadingbarActive = false;
local cameraOrigin = TitlescreenSceneCamera;

-- Generic functions
local function ScaleUDim2(udim: UDim2, scalar: number): UDim2
	return UDim2.new(udim.X.Scale * scalar, udim.X.Offset * scalar, udim.Y.Scale * scalar, udim.Y.Offset * scalar);	
end

-- Functions 
local function TitleScreenClose()
	-- prevent procedure from happening if title screen isnt active
	if (not active) then return end;
	
	Camera.CameraType = Enum.CameraType.Custom; 
	-- Reset camera type (custom is placeholder until character spawning / intro scenes are finalized)
	
	SpawnEvent:FireServer();
	-- Tell server to begin spawning player
	
	ScreenGUI.Enabled = false;
	active = false;
	loadingbarActive = false;
	-- Reset assets
	
	StarterGui:SetCoreGuiEnabled(Enum.CoreGuiType.All, true);
	-- Re enable core gui
	
	-- Reset DOF
	NormalDOF.Enabled = true;
	SceneDOF.Enabled = false;
	
	-- Stop Music
	Song:Stop();
end

local function ContentLoadedCompleted()
	if (not loadingbarActive) then return end; 
	loadingbarActive = false;
	
	-- Fade out loading bar
	local LoadingBarTween = TweenService:Create(LoadingBarCanvas, LoadingBarTweenInfo__Completed, {GroupTransparency = 1});
	LoadingBarTween.Completed:Once(function()
		-- Transition scene
		local Transition1 = TweenService:Create(MainFrame, TipChangeTweenInfo, {BackgroundTransparency = 0});
		local Transition2 = TweenService:Create(MainFrame, TipChangeTweenInfo, {BackgroundTransparency = 1});
		Transition1:Play();
		Transition1.Completed:Wait();
		cameraOrigin = PlayscreenSceneCamera;
		task.wait(0.1);
		Transition2:Play();
		
		-- Enable and fade in play button once bar had faded out
		TweenService:Create(PlayButtonCanvas, SceenChangeTweenInfo, {GroupTransparency = 0}):Play();
		TweenService:Create(UpdateLogCanvas, SceenChangeTweenInfo, {GroupTransparency = 0}):Play();
		
		-- Grow play button 
		task.spawn(function()
			local elapsed = 0;
			while (active) do 
				elapsed += RunService.PreRender:Wait();
				local scalar = 1 + math.sin(elapsed * math.pi * PlayButtonScalarSpeed / 2) * PlayButtonScalarOffset;
				PlayButtonCanvas.Size = ScaleUDim2(OriginPlaySize, scalar);
			end
		end)
	end)
	
	-- Delay and change scene
	task.wait(SceenChangeDelay);
	LoadingBarTween:Play();
end

local function ContentLoadedAsync()
	assetsLoaded += 1; 
	if (not loadingbarActive) then return end;
	
	-- Update loading bar
	local percent = assetsLoaded / assetsToLoad;
	TweenService:Create(LoadingBar, LoadingBarTweenInfo__PercentChanged, {Size = UDim2.new(percent, 0, 1, 0)}):Play();
	
	-- if all assets are loaded, begin procedure for animating out loading bar
	if (assetsLoaded >= assetsToLoad) then 
		ContentLoadedCompleted();
	end
end

local function CreateTipList(): {[number]: ImageLabel} 
	-- Clones tip list without cloning assets with table.clone
	local out = {};
	for i,v in pairs(Tips) do 
		out[i] = v;
	end
	return out;
end


local function CameraSway()
	-- Generate camera sway
	local current = Vector3.zero;
	local goal = Vector3.new(math.random(-MaxCameraOffset, MaxCameraOffset), math.random(-MaxCameraOffset, MaxCameraOffset), 0);
	local delta = goal;
	local elapsed = 0;
	local timeAmount = delta.Magnitude / CameraSwaySpeed;
	
	-- Loop
	while (active) do 
		-- Find alpha
		elapsed = math.min(elapsed + RunService.PreRender:Wait(), timeAmount);
		local alpha = TweenService:GetValue(elapsed / timeAmount, Enum.EasingStyle.Sine, Enum.EasingDirection.InOut);
		local currentOffset = current + delta * alpha;
		
		-- Move camera
		Camera.CFrame = cameraOrigin.CFrame * CFrame.new(currentOffset);
		
		-- If time has elapsed generate new sway
		if (elapsed == timeAmount) then 
			current = goal;
			goal = Vector3.new(math.random(-MaxCameraOffset, MaxCameraOffset), math.random(-MaxCameraOffset, MaxCameraOffset), 0);
			delta = goal - current;
			elapsed = 0;
			timeAmount = delta.Magnitude / CameraSwaySpeed;
		end
	end
end

local function TipProcedure()
	-- Clone tip list to empty table so when tip is displayed it can be removed from list
	local CurrentTipSet = CreateTipList();
	
	-- Starting tip
	local currentTip = math.random(1, TipAmount);
	CurrentTipSet[currentTip].Visible = true;
	CurrentTipSet[currentTip].ImageTransparency = 1;
	TweenService:Create(CurrentTipSet[currentTip], TipChangeTweenInfo, {ImageTransparency = 0}):Play();
	local lastTipAsset = CurrentTipSet[currentTip];
	table.remove(CurrentTipSet, currentTip);
	
	-- Loop
	local tipAmount = TipAmount - 1;
	local elapsed = 0;
	while (loadingbarActive) do 
		elapsed = math.min(elapsed + RunService.PreRender:Wait(), TipChangeSpeed);
		
		if (elapsed == TipChangeSpeed) then 
			-- Create new list once all items are shown
			if (tipAmount <= 0) then 
				CurrentTipSet = CreateTipList();
				tipAmount = TipAmount;
			end
			
			-- Disable old tip
			local tweenOutTip = lastTipAsset;
			local tipFadeOutTween = TweenService:Create(tweenOutTip, TipChangeTweenInfo, {ImageTransparency = 1});

			-- Find new tip to display
			currentTip = math.random(1, tipAmount);
			repeat currentTip = math.random(1, tipAmount) until CurrentTipSet[currentTip] ~= tweenOutTip; -- prevents empty tip bar
			
			CurrentTipSet[currentTip].Visible = true;
			lastTipAsset = CurrentTipSet[currentTip];
			table.remove(CurrentTipSet, currentTip);
			
			-- Fade in new tip
			lastTipAsset.ImageTransparency = 1;
			tipFadeOutTween.Completed:Once(function()
				tweenOutTip.Visible = false;
				TweenService:Create(lastTipAsset, TipChangeTweenInfo, {ImageTransparency = 0}):Play();
			end)
			tipFadeOutTween:Play();
			
			-- Change Values
			tipAmount = math.max(tipAmount - 1, 0);
			elapsed = 0;
		end
	end
	
	-- Fade out tips
	task.wait(SceenChangeDelay);
	TweenService:Create(lastTipAsset, LoadingBarTweenInfo__Completed, {ImageTransparency = 1}):Play();
	TweenService:Create(TipBar, LoadingBarTweenInfo__Completed, {ImageTransparency = 1}):Play();
end

local function PlayButtonClicked()
	if (loadingbarActive) then return end;
	if (not active) then return end;
	TitleScreenClose();
end

-- Events
UIS.InputBegan:Connect(function(x, processed)
	if (processed) then return end;
	PlayButtonClicked();
end)

-- Module
local controller = {
	ContentLoadedAsync = ContentLoadedAsync,
	EndLoadingProcedure = TitleScreenClose,
	BeginLoadingProcedure = function(assetLoad: number)
		-- Preload all content for title screen ui
		
		-- Disable core gui
		StarterGui:SetCoreGuiEnabled(Enum.CoreGuiType.All, false);
	
		-- Reset assets 
		ScreenGUI.Enabled = true;
		LoadingBar.Size = UDim2.new(0,0,1,0);
		TipBar.ImageTransparency = 0;
		LoadingBarCanvas.GroupTransparency = 0;
		PlayButtonCanvas.GroupTransparency = 1;
		UpdateLogCanvas.GroupTransparency = 1;
		
		-- Change DOF
		NormalDOF.Enabled = false;
		SceneDOF.Enabled = true;
		
		-- Disable all tips
		for _, tip in pairs(Tips) do 
			tip.Visible = false;
		end
		
		-- Update parameters
		assetsToLoad = assetLoad;
		assetsLoaded = 0;
		loadingbarActive = true;
		active = true;
		
		-- Begin tip procedure
		task.spawn(TipProcedure);
		
		-- Begin camera sway
		task.spawn(CameraSway);
		
		-- Move camera to scene 
		Camera.CameraType = Enum.CameraType.Scriptable;
		cameraOrigin = TitlescreenSceneCamera;
		
		-- Play Music
		Song:Play();
	end,	
};

return controller;
