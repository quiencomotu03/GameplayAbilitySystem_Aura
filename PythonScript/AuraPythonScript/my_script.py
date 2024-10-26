import unreal

# 현재 레벨에 있는 모든 액터 가져오기
actors = unreal.EditorLevelLibrary.get_all_level_actors()

for actor in actors:
    print(f"Actor found: {actor.get_name()}")