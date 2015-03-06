
function update(obj, current_time)
	local p = obj:pos()
	renderer.set_cam_pos(-p:x(), p:y() + 500)
end
