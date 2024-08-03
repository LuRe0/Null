
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local Player_Behavior = {
    data = {
        health = { value = 100, serialize = false },
        moveSpeed = { value = 1, serialize = true },
        name = { value = "Deez", serialize = false },
        isAlive = { value = true, serialize = true }
    }
}

setmetatable(Player_Behavior, {
    __index = function(t, k)
        if Player_Behavior.data[k] then
            return Player_Behavior.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if Player_Behavior.data[k] then
            Player_Behavior.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})



function Player_Behavior:Start()
    -- print("Player behavior started")
    -- Initialization code here
end

function Player_Behavior:Update(dt)
    -- local transform = ParentEntity.GetTranform()

    -- if(Input.KeyPressed(KEY_UP)) then 
    --     print("UP")
    -- end
    print(Player_Behavior.health)
end

function Player_Behavior:Exit()
    -- print("Player behavior exited")
    -- Cleanup code here
end
 
return Player_Behavior
