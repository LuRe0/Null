
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local BlockBehaviour = {
    data = 
    {
        name = { value = "BlockBehaviour", serialize = true }
    }
}

setmetatable(BlockBehaviour, {
    __index = function(t, k)
        if BlockBehaviour.data[k] then
            return BlockBehaviour.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if BlockBehaviour.data[k] then
            BlockBehaviour.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function BlockBehaviour:Start()
    -- print("BlockBehaviour started")
    -- Initialization code here
end

function BlockBehaviour:Update(dt)
    -- print("BlockBehaviour started")
    -- Update code here
end

function BlockBehaviour:Exit()
    -- print("BlockBehaviour exited")
    -- Cleanup code here
end



function BlockBehaviour:OnCollisionEnter(otherEntity)
    -- Check if otherEntity is valid
    if otherEntity then
        if(otherEntity.name == "ball") then
            pEntity:destroy()
        end
    end
end


return BlockBehaviour
