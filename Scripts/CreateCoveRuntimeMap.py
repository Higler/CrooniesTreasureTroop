import unreal

level_path = "/Game/Cove/Maps/CoveRuntimeMap"
unreal.EditorLevelLibrary.new_level(level_path)

world = unreal.EditorLevelLibrary.get_editor_world()

def spawn_actor(actor_class, location, rotation=(0.0, 0.0, 0.0)):
    return unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class,
        unreal.Vector(*location),
        unreal.Rotator(*rotation),
    )

spawn_actor(unreal.PlayerStart, (0.0, 0.0, 120.0))

directional = spawn_actor(unreal.DirectionalLight, (-600.0, -400.0, 900.0), (-45.0, -35.0, 0.0))
directional.get_editor_property("light_component").set_editor_property("intensity", 4.0)

sky = spawn_actor(unreal.SkyLight, (0.0, 0.0, 500.0))
sky.get_editor_property("light_component").set_editor_property("intensity", 0.8)

unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
unreal.EditorLevelLibrary.save_current_level()
