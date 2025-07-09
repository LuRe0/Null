local BlockManager = {
	data = {
		rows = { value = 5, serialize = true },
		cols = { value = 10, serialize = true },
		blockSpacing = { value = vec2(64,16), serialize = true },
		blockName = { value = "Block", serialize = true },
		moveSpeed = { value = 20, serialize = true },
		spawnCooldown = { value = 2.0, serialize = true },
		timer = { value = 0, serialize = false },
		spawnOffset = { value = vec2(0, 0), serialize = true },
		delayPerBlock = { value = 0.2, serialize = true },
		decaySpeed = { value = 1.5, serialize = true },
		maxStrength = { value = 2, serialize = true },
		blocks = { value = {}, serialize = true }
	}
}

setmetatable(BlockManager, {
	__index = function(t, k)
		if BlockManager.data[k] then
			return BlockManager.data[k].value
		else
			return nil
		end
	end,
	__newindex = function(t, k, v)
		if BlockManager.data[k] then
			BlockManager.data[k].value = v
		else
			rawset(t, k, v)
		end
	end
})

function BlockManager:Start()
	self:SpawnGrid()
    
end

function BlockManager:Update(dt)
	self:WaveEffect(dt)
end

function BlockManager:Exit()

end


function BlockManager:OnReload()
	self:UpdateBlocks()
end


function BlockManager:WaveEffect(dt)
	BlockManager.timer = BlockManager.timer + dt

    local delayPerBlock = BlockManager.delayPerBlock
	local decaySpeed = BlockManager.decaySpeed
	local maxStrength = BlockManager.maxStrength
	local totalBlocks = BlockManager.rows * BlockManager.cols
	local totalDuration = totalBlocks * delayPerBlock + (maxStrength / decaySpeed)

	if BlockManager.timer >= totalDuration then
		BlockManager.timer = BlockManager.timer - totalDuration
	end

	local currentTime = BlockManager.timer

	for row = 0, #BlockManager.blocks - 1 do
		for col = 0, BlockManager.cols - 1 do
			local block = BlockManager.blocks[row][col]
			if block and not block:is_destroyed() then
				local sprite = block:get_component(Sprite)
				if sprite then
					local index = row * BlockManager.cols + col
					local activationTime = index * delayPerBlock
					local timeSinceActivated = currentTime - activationTime

					if timeSinceActivated >= 0 then
						local strength = math.max(0.0, maxStrength - timeSinceActivated * decaySpeed)

						if math.abs(sprite.emissive_strength - strength) > 0.01 then
							local baseColor = sprite.tint
							baseColor.a = 1.0
							sprite:set_tint(baseColor)
							sprite:set_emissive_color(baseColor)
							sprite:set_emissive_strength(strength)
						end
					elseif sprite.emissive_strength ~= 0.0 then
						sprite:set_emissive_strength(0.0)
					end
				end
			end
		end
	end
end




function BlockManager:OnValueChangedInEditor(propertyName)
	Trace.debug("{}", propertyName)
	if propertyName == "rows" or propertyName == "cols" then
		self:SpawnGrid()
	else
		self:UpdateBlocks()
	end
end

function BlockManager:SpawnGrid()   
	for i = 0, BlockManager.rows - 1 do
		BlockManager.blocks[i] = {}
		for j = 0, BlockManager.cols - 1 do
            local x = BlockManager.spawnOffset.x + j * BlockManager.blockSpacing.x
			local y = BlockManager.spawnOffset.y + (BlockManager.rows - 1 - i) * BlockManager.blockSpacing.y
            local block = Instantiate(BlockManager.blockName)
            
            if block and block:has_component(Transform) then
				local t = block:get_component(Transform)
				t:set_translation(vec3(x, y, 0))
                self:ColorBlock(block)
                if BlockManager.blocks  then
                    BlockManager.blocks[i][j] = block
                end
			end       
		end
	end
end


function BlockManager:UpdateBlocks()
    for i = 0, BlockManager.rows - 1 do
        for j = 0, BlockManager.cols - 1 do
            local block = BlockManager.blocks[i] and BlockManager.blocks[i][j]
            if block and block:has_component(Transform) then
                local t = block:get_component(Transform)
                local x = BlockManager.spawnOffset.x + j * BlockManager.blockSpacing.x
                local y = BlockManager.spawnOffset.y + (BlockManager.rows - 1 - i) * BlockManager.blockSpacing.y
                t:set_translation(vec3(x, y, 0))
                self:ColorBlock(block)
            end
        end
    end
end



function BlockManager:ColorBlock(block)
	if block and block:has_component(Sprite) then
		local sprite = block:get_component(Sprite)
		local r = Random.range_float(0.1, 1.0)
		local g = Random.range_float(0.1, 1.0)
		local b = Random.range_float(0.1, 1.0)
		sprite:set_tint(vec4(r, g, b, 1.0))
	end
end

return BlockManager
