import unreal
for path in ['/Game/Croonies/Characters/TedBoy/SM_TedBoy_Base.SM_TedBoy_Base','/Game/Croonies/Characters/TedBoy/ted_boy.ted_boy']:
    asset=unreal.EditorAssetLibrary.load_asset(path)
    cls=asset.get_class().get_name() if asset else 'None'
    unreal.log(f'TEDBOY_INSPECT path={path} class={cls} asset={asset}')
    if asset and cls == 'StaticMesh':
        try:
            unreal.log(f'TEDBOY_INSPECT bounds={asset.get_bounding_box()}')
        except Exception as e:
            unreal.log(f'TEDBOY_INSPECT bounds_error={e}')
