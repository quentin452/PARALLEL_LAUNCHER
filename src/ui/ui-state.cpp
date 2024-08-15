#include "src/ui/ui-state.hpp"

const UiState UiState::Default = {
	/* romList */ {
		/* selectedRom */ fs::path(),
		/* selectedGroup */ "",
		/* expandedGroups */ std::vector<string>()
	},
	/* rhdcList */ {
		/* sortBy */ RhdcSorting::Name,
		/* sortAsc */ true,
		/* playlist */ "",
		/* hackId */ ""
	},
	/* classicViewSize */ {
		/* width */ 450,
		/* height */ 600
	},
	/* rhdcViewSize */ {
		/* width */ 1280,
		/* height */ 700
	},
	/* showAllPlugins */ false,
	/* rhdcView */ false
};

static constexpr char P_SELECTED_ROM[] = "selected_rom";
static constexpr char P_SELECTED_ROM_TAG[] = "selected_rom_tag";
static constexpr char P_EXPANDED_GROUPS[] = "expanded_groups";
static constexpr char P_CLASSIC_WINDOW[] = "classic_window";
static constexpr char P_RHDC_WINDOW[] = "rhdc_window";
static constexpr char P_SHOW_ALL_PLUGINS[] = "show_all_plugins";
static constexpr char P_RHDC_SORT_BY[] = "rhdc_sort_by";
static constexpr char P_RHDC_ASC[] = "rhdc_ascending";
static constexpr char P_RHDC_PLAYLIST[] = "rhdc_filter";
static constexpr char P_RHDC_HACK[] = "rhdc_selected_hack";
static constexpr char P_RHDC_VIEW[] = "rhdc_view";
static constexpr char P_WIDTH[] = "width";
static constexpr char P_HEIGHT[] = "height";
/* Legacy properties */
static constexpr char P_WINDOW_WIDTH[] = "window_width";
static constexpr char P_WINDOW_HEIGHT[] = "window_height";

template<> void JsonSerializer::serialize<UiState>( JsonWriter &jw, const UiState &obj ) {
	jw.writeObjectStart();
	jw.writeProperty( P_SELECTED_ROM, obj.romList.selectedRom.u8string() );
	jw.writeProperty( P_SELECTED_ROM_TAG, obj.romList.selectedGroup );
	jw.writePropertyName( P_EXPANDED_GROUPS );
	jw.writeArrayStart();
	for( const string &group : obj.romList.expandedGroups ) {
		jw.writeString( group );
	}
	jw.writeArrayEnd();
	jw.writePropertyName( P_CLASSIC_WINDOW );
	jw.writeObjectStart();
		jw.writeProperty( P_WIDTH, obj.classicViewSize.width );
		jw.writeProperty( P_HEIGHT, obj.classicViewSize.height );
	jw.writeObjectEnd();
	jw.writePropertyName( P_RHDC_WINDOW );
	jw.writeObjectStart();
		jw.writeProperty( P_WIDTH, obj.rhdcViewSize.width );
		jw.writeProperty( P_HEIGHT, obj.rhdcViewSize.height );
	jw.writeObjectEnd();
	jw.writeProperty( P_SHOW_ALL_PLUGINS, obj.showAllPlugins );
	jw.writeProperty( P_RHDC_VIEW, obj.rhdcView );
	jw.writeProperty( P_RHDC_SORT_BY, obj.rhdcList.sortBy );
	jw.writeProperty( P_RHDC_ASC, obj.rhdcList.sortAsc );
	jw.writeProperty( P_RHDC_PLAYLIST, obj.rhdcList.playlist );
	jw.writeProperty( P_RHDC_HACK, obj.rhdcList.hackId );
	jw.writeObjectEnd();
}

template<> UiState JsonSerializer::parse<UiState>( const Json &json ) {
	UiState state = {
		{
			fs::to_path( json[P_SELECTED_ROM].getOrDefault<string>( "" ) ),
			json[P_SELECTED_ROM_TAG].getOrDefault<string>( "" ),
			std::vector<string>()
		},
		{
			json[P_RHDC_SORT_BY].getOrDefault<RhdcSorting>( UiState::Default.rhdcList.sortBy ),
			json[P_RHDC_ASC].getOrDefault<bool>( UiState::Default.rhdcList.sortAsc ),
			json[P_RHDC_PLAYLIST].getOrDefault<string>( UiState::Default.rhdcList.playlist ),
			json[P_RHDC_HACK].getOrDefault<string>( UiState::Default.rhdcList.hackId )
		},
		{
			json[P_CLASSIC_WINDOW][P_WIDTH].getOrDefault<int>( UiState::Default.classicViewSize.width ),
			json[P_CLASSIC_WINDOW][P_HEIGHT].getOrDefault<int>( UiState::Default.classicViewSize.height )
		},
		{
			json[P_RHDC_WINDOW][P_WIDTH].getOrDefault<int>( UiState::Default.rhdcViewSize.width ),
			json[P_RHDC_WINDOW][P_HEIGHT].getOrDefault<int>( UiState::Default.rhdcViewSize.height )
		},
		json[P_SHOW_ALL_PLUGINS].getOrDefault<bool>( UiState::Default.showAllPlugins ),
		json[P_RHDC_VIEW].getOrDefault<bool>( UiState::Default.rhdcView )
	};

	if( json[P_EXPANDED_GROUPS].isArray() ) {
		const JArray &groupArray = json[P_EXPANDED_GROUPS].array();
		state.romList.expandedGroups.reserve( groupArray.size() );

		for( const Json &group : groupArray ) {
			state.romList.expandedGroups.push_back( group.get<string>() );
		}
	}

	// Legacy format
	if( json[P_WINDOW_WIDTH].exists() && json[P_WINDOW_HEIGHT].exists() && !json[P_CLASSIC_WINDOW].exists() ) {
		state.classicViewSize.width = state.rhdcViewSize.width = json[P_WINDOW_WIDTH].get<int>();
		state.classicViewSize.height = state.rhdcViewSize.height = json[P_WINDOW_HEIGHT].get<int>();
	}

	return state;
}
