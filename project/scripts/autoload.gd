extends Node

func _ready() -> void:
	print("=== Godot Qt Startup ===")
	print("Engine Version: ", Engine.get_version_info()["string"])
	print("========================")
	
	var root = get_tree().root
	print("Root viewport size: ", root.get_visible_rect().size)
	
