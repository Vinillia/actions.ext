#include "utils.h"
#include "extension.h"
#include "actions_helpers.h"
#include "actions_procs.h"
#include "actions_cquery.h"

const std::map<std::string, int32_t>* g_CachedOffsets;

OffsetManager* GetOffsetsManager()
{
	static OffsetManager offsmgr(CONFIG_FILE_NAME);
	return &offsmgr;
}

void ExecuteProcessor(CBaseEntity* entity, Action<void>* action)
{
	ActionContextualProcessor processor(entity, action);

    if (processor.ShouldStartProcessors())
        processor.StartProcessors();
}

const bool ConfigureHooks()
{
    return ActionContextualProcessor::ConfigureHooks();
}