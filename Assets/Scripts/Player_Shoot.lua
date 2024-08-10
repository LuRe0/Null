
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local Player_Shoot = {
    data = 
    {
        name = { value = "Player_Shoot", serialize = true }
    }
}

setmetatable(Player_Shoot, {
    __index = function(t, k)
        if Player_Shoot.data[k] then
            return Player_Shoot.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if Player_Shoot.data[k] then
            Player_Shoot.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function Player_Shoot:Start()
    -- print("Player_Shoot started")
    -- Initialization code here
end

function Player_Shoot:Update(dt)
    -- print("Player_Shoot started")
    -- Update code here
end

function Player_Shoot:Exit()
    -- print("Player_Shoot exited")
    -- Cleanup code here
end

return Player_Shoot
