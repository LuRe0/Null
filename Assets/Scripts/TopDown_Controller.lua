
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local TopDown_Controller = {
    data = 
    {
        name = { value = "TopDown_Controller", serialize = true }
    }
}

setmetatable(TopDown_Controller, {
    __index = function(t, k)
        if TopDown_Controller.data[k] then
            return TopDown_Controller.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if TopDown_Controller.data[k] then
            TopDown_Controller.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function TopDown_Controller:Start()
    -- print("TopDown_Controller started")
    -- Initialization code here
end

function TopDown_Controller:Update(dt)

    local inputX, inputY = 0, 0

    local movementController = pEntity:get_component(CharacterMovement)
    local transform = pEntity:get_component(Transform)

    if(movementController) then
        if Input.KeyDown(KEY_W) then
            inputY = inputY + 1
        end
        if Input.KeyDown(KEY_S) then
            inputY = inputY - 1
        end
        if Input.KeyDown(KEY_D) then
            inputX = inputX + 1
        end
        if Input.KeyDown(KEY_A) then
            inputX = inputX - 1
        end
    end

    
    -- Movement direction in world space
    local moveDir = transform.forward * inputY 
    -- local rot =  transform.right * inputX;

    -- Normalize to prevent diagonal speed boost
    if length(moveDir) > 0 then
        moveDir = normalize(moveDir)
    end
    
    
    movementController:set_move_input(moveDir.x, moveDir.y)
    Trace.debug("{0}, {1}", inputX, inputY)
    movementController:set_rotate_input(inputX, 0)
end

function TopDown_Controller:Exit()
    -- print("TopDown_Controller exited")
    -- Cleanup code here
end

return TopDown_Controller
