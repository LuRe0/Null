
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local Does Something = {
    data = 
    {
        name = { value = "Does Something", serialize = true }
    }
}

setmetatable(Does Something, {
    __index = function(t, k)
        if Does Something.data[k] then
            return Does Something.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if Does Something.data[k] then
            Does Something.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function Does Something:Start()
    -- print("Does Something started")
    -- Initialization code here
end

function Does Something:Update(dt)
    -- print("Does Something started")
    -- Update code here
end

function Does Something:Exit()
    -- print("Does Something exited")
    -- Cleanup code here
end

return Does Something
