
static const char _afb_description_v2_low_can[] =
    "{\"openapi\":\"3.0.0\",\"$schema\":\"http:iot.bzh/download/openapi/schem"
    "a-3.0/default-schema.json\",\"info\":{\"description\":\"\",\"title\":\"l"
    "ow-can\",\"version\":\"4.0\",\"x-binding-c-generator\":{\"api\":\"low-ca"
    "n\",\"version\":2,\"prefix\":\"\",\"postfix\":\"\",\"start\":null,\"onev"
    "ent\":null,\"init\":\"initv2\",\"scope\":\"\",\"private\":false}},\"serv"
    "ers\":[{\"url\":\"ws://{host}:{port}/api/monitor\",\"description\":\"Low"
    " level CAN API.\",\"variables\":{\"host\":{\"default\":\"localhost\"},\""
    "port\":{\"default\":\"1234\"}},\"x-afb-events\":[{\"$ref\":\"#/component"
    "s/schemas/afb-event\"}]}],\"components\":{\"schemas\":{\"afb-reply\":{\""
    "$ref\":\"#/components/schemas/afb-reply-v2\"},\"afb-event\":{\"$ref\":\""
    "#/components/schemas/afb-event-v2\"},\"afb-reply-v2\":{\"title\":\"Gener"
    "ic response.\",\"type\":\"object\",\"required\":[\"jtype\",\"request\"],"
    "\"properties\":{\"jtype\":{\"type\":\"string\",\"const\":\"afb-reply\"},"
    "\"request\":{\"type\":\"object\",\"required\":[\"status\"],\"properties\""
    ":{\"status\":{\"type\":\"string\"},\"info\":{\"type\":\"string\"},\"toke"
    "n\":{\"type\":\"string\"},\"uuid\":{\"type\":\"string\"},\"reqid\":{\"ty"
    "pe\":\"string\"}}},\"response\":{\"type\":\"object\"}}},\"afb-event-v2\""
    ":{\"type\":\"object\",\"required\":[\"jtype\",\"event\"],\"properties\":"
    "{\"jtype\":{\"type\":\"string\",\"const\":\"afb-event\"},\"event\":{\"ty"
    "pe\":\"string\"},\"data\":{\"type\":\"object\"}}}},\"x-permissions\":{\""
    "monitor\":{\"permission\":\"urn:AGL:permission:low-can:public:monitor\"}"
    ",\"write\":{\"permission\":\"urn:AGL:permission::platform:can:write \"}}"
    ",\"responses\":{\"200\":{\"description\":\"A complex object array respon"
    "se\",\"content\":{\"application/json\":{\"schema\":{\"$ref\":\"#/compone"
    "nts/schemas/afb-reply\"}}}}}},\"paths\":{\"/subscribe\":{\"description\""
    ":\"Subscribe to CAN signals events\",\"parameters\":[{\"in\":\"query\",\""
    "name\":\"event\",\"required\":false,\"schema\":{\"type\":\"string\"}}],\""
    "responses\":{\"200\":{\"$ref\":\"#/components/responses/200\"}}},\"/unsu"
    "bscribe\":{\"description\":\"Unsubscribe previously suscribed signals.\""
    ",\"parameters\":[{\"in\":\"query\",\"name\":\"event\",\"required\":false"
    ",\"schema\":{\"type\":\"string\"}}],\"responses\":{\"200\":{\"$ref\":\"#"
    "/components/responses/200\"}}},\"/get\":{\"description\":\"get a current"
    " value of CAN message\",\"parameters\":[{\"in\":\"query\",\"name\":\"eve"
    "nt\",\"required\":false,\"schema\":{\"type\":\"string\"}}],\"responses\""
    ":{\"200\":{\"$ref\":\"#/components/responses/200\"}}},\"/list\":{\"descr"
    "iption\":\"get a supported CAN message list\",\"responses\":{\"200\":{\""
    "$ref\":\"#/components/responses/200\"}}},\"/auth\":{\"description\":\"Au"
    "thenticate session to be raise Level Of Assurance.\",\"get\":{\"x-permis"
    "sions\":{\"$ref\":\"#/components/x-permissions/write\"},\"responses\":{\""
    "200\":{\"$ref\":\"#/components/responses/200\"}}}},\"/write\":{\"descrip"
    "tion\":\"Write a CAN messages to the CAN bus.\",\"get\":{\"x-permissions"
    "\":{\"LOA\":1},\"parameters\":[{\"in\":\"query\",\"name\":\"bus_name\",\""
    "required\":false,\"schema\":{\"type\":\"string\"}},{\"in\":\"query\",\"n"
    "ame\":\"frame\",\"required\":false,\"schema\":{\"type\":\"array\"},\"par"
    "ameters\":[{\"in\":\"query\",\"name\":\"can_id\",\"required\":false,\"sc"
    "hema\":{\"type\":\"string\"}},{\"in\":\"query\",\"name\":\"can_dlc\",\"r"
    "equired\":false,\"schema\":{\"type\":\"string\"}},{\"in\":\"query\",\"na"
    "me\":\"can_data\",\"required\":false,\"schema\":{\"type\":\"string\"}}]}"
    ",{\"in\":\"query\",\"name\":\"signal_name\",\"required\":false,\"schema\""
    ":{\"type\":\"string\"}},{\"in\":\"query\",\"name\":\"signal_value\",\"re"
    "quired\":false,\"schema\":{\"type\":\"integer\"}}],\"responses\":{\"200\""
    ":{\"$ref\":\"#/components/responses/200\"}}}}}}"
;

static const struct afb_auth _afb_auths_v2_low_can[] = {
	afb::auth_permission("urn:AGL:permission::platform:can:write ")
};

 void subscribe(struct afb_req req);
 void unsubscribe(struct afb_req req);
 void get(struct afb_req req);
 void list(struct afb_req req);
 void auth(struct afb_req req);
 void write(struct afb_req req);

static const struct afb_verb_v2 _afb_verbs_v2_low_can[] = {
    {
        .verb = "subscribe",
        .callback = subscribe,
        .auth = NULL,
        .info = "Subscribe to CAN signals events",
        .session = AFB_SESSION_NONE_V2
    },
    {
        .verb = "unsubscribe",
        .callback = unsubscribe,
        .auth = NULL,
        .info = "Unsubscribe previously suscribed signals.",
        .session = AFB_SESSION_NONE_V2
    },
    {
        .verb = "get",
        .callback = get,
        .auth = NULL,
        .info = "get a current value of CAN message",
        .session = AFB_SESSION_NONE_V2
    },
    {
        .verb = "list",
        .callback = list,
        .auth = NULL,
        .info = "get a supported CAN message list",
        .session = AFB_SESSION_NONE_V2
    },
    {
        .verb = "auth",
        .callback = auth,
        .auth = &_afb_auths_v2_low_can[0],
        .info = "Authenticate session to be raise Level Of Assurance.",
        .session = AFB_SESSION_NONE_V2
    },
    {
        .verb = "write",
        .callback = write,
        .auth = NULL,
        .info = "Write a CAN messages to the CAN bus.",
        .session = AFB_SESSION_LOA_1_V2
    },
    {
        .verb = NULL,
        .callback = NULL,
        .auth = NULL,
        .info = NULL,
        .session = 0
	}
};

const struct afb_binding_v2 afbBindingV2 = {
    .api = "low-can",
    .specification = _afb_description_v2_low_can,
    .info = "",
    .verbs = _afb_verbs_v2_low_can,
    .preinit = NULL,
    .init = initv2,
    .onevent = NULL,
    .noconcurrency = 0
};

