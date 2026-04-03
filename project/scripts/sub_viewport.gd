extends SubViewport

func _ready() -> void:
	pass

func _process(delta: float) -> void:
	pass

func get_texture_image() -> Image:
	return get_texture().get_image()
