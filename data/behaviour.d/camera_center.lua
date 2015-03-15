
function update(obj, current_time)
	local p = obj:pos()
	local view = renderer.viewport()
	renderer.set_cam_pos(-p:x() + (view:width() * 0.5), p:y() - (view:height() * 0.5))
end
