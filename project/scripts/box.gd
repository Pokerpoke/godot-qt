extends CSGBox3D

# 旋转速度（弧度/秒）
var rotation_speed = 1.0

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	# 绕 Y 轴旋转（可以根据需要修改轴）
	rotation.y += rotation_speed * delta
