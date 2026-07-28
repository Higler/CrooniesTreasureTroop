from pathlib import Path
import unreal

source = Path(r"F:\gooniesgame\SourceAssets\TedBoy\ted_boy.obj")

task = unreal.AssetImportTask()
task.filename = str(source)
task.destination_path = "/Game/Croonies/Characters/TedBoy"
task.destination_name = "SM_TedBoy_Base"
task.automated = True
task.replace_existing = True
task.save = True

options = unreal.FbxImportUI()
options.import_mesh = True
options.import_as_skeletal = False
options.import_materials = True
options.import_textures = True
options.static_mesh_import_data.combine_meshes = True
options.static_mesh_import_data.generate_lightmap_u_vs = True
options.static_mesh_import_data.auto_generate_collision = False
task.options = options

unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
unreal.EditorAssetLibrary.save_directory("/Game/Croonies/Characters/TedBoy", only_if_is_dirty=False, recursive=True)
print("Imported Ted Boy base model to /Game/Croonies/Characters/TedBoy/SM_TedBoy_Base")
