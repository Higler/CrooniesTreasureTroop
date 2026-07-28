import re
from pathlib import Path
import unreal

SOURCE_DIR = Path(r"F:\gooniesgame\icon images")

LEVEL_PREFIXES = {
    "Level 1": "/Game/Cove/LevelCards",
    "Level 2": "/Game/Cove/LevelCards",
    "Level 3": "/Game/Cove/LevelCards",
    "Level 4": "/Game/Cove/LevelCards",
    "Level 5": "/Game/Cove/LevelCards",
    "Level 6": "/Game/Cove/LevelCards",
    "Level 7": "/Game/Cove/LevelCards",
    "Level 8": "/Game/Cove/LevelCards",
    "Level 9": "/Game/Cove/LevelCards",
    "Level 10": "/Game/Cove/LevelCards",
}

BRANDING = {"loadingscreen.jpg", "team.jpg", "treasurepile.jpg", "treasurechest.jpg"}


def clean_name(path: Path) -> str:
    stem = re.sub(r"[^A-Za-z0-9_]+", "_", path.stem)
    return re.sub(r"_+", "_", stem).strip("_")


def destination_for(path: Path) -> str:
    for prefix, destination in LEVEL_PREFIXES.items():
        if path.name.startswith(prefix):
            return destination
    if path.name in BRANDING:
        return "/Game/Cove/Branding"
    return "/Game/Cove/UIIcons"


tasks = []
for source in sorted(list(SOURCE_DIR.glob("*.jpg")) + list(SOURCE_DIR.glob("*.png"))):
    task = unreal.AssetImportTask()
    task.filename = str(source)
    task.destination_path = destination_for(source)
    task.destination_name = clean_name(source)
    task.automated = True
    task.replace_existing = True
    task.save = True
    tasks.append(task)

unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks(tasks)
unreal.EditorAssetLibrary.save_directory("/Game/Cove", only_if_is_dirty=False, recursive=True)
print(f"Imported {len(tasks)} image assets into /Game/Cove")

