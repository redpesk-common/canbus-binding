
static const char _afb_description_v2_low_can[] =
    "{\"openapi\":\"3.0.0\",\"$schema\":\"http:iot.bzh/download/openapi/schem"
    "a-3.0/default-schema.json\",\"info\":{\"description\":\"\",\"title\":\"l"
    "ow-level-can-service\",\"version\":\"4.0\",\"x-binding-c-generator\":{\""
    "api\":\"low-can\",\"version\":2,\"prefix\":\"\",\"postfix\":\"\",\"start"
    "\":null,\"onevent\":null,\"init\":\"initv2\",\"scope\":\"\",\"private\":"
    "false}},\"servers\":[{\"url\":\"ws://{host}:{port}/api/monitor\",\"descr"
    "iption\":\"Low level CAN API.\",\"variables\":{\"host\":{\"default\":\"l"
    "ocalhost\"},\"port\":{\"default\":\"1234\"}},\"x-afb-events\":[{\"$ref\""
    ":\"#/components/schemas/afb-event\"}]}],\"components\":{\"schemas\":{\"a"
    "fb-reply\":{\"$ref\":\"#/components/schemas/afb-reply-v2\"},\"afb-event\""
    ":{\"$ref\":\"#/components/schemas/afb-event-v2\"},\"afb-reply-v2\":{\"ti"
    "tle\":\"Generic response.\",\"type\":\"object\",\"required\":[\"jtype\","
    "\"request\"],\"properties\":{\"jtype\":{\"type\":\"string\",\"const\":\""
    "afb-reply\"},\"request\":{\"type\":\"object\",\"required\":[\"status\"],"
    "\"properties\":{\"status\":{\"type\":\"string\"},\"info\":{\"type\":\"st"
    "ring\"},\"token\":{\"type\":\"string\"},\"uuid\":{\"type\":\"string\"},\""
    "reqid\":{\"type\":\"string\"}}},\"response\":{\"type\":\"object\"}}},\"a"
    "fb-event-v2\":{\"type\":\"object\",\"required\":[\"jtype\",\"event\"],\""
    "properties\":{\"jtype\":{\"type\":\"string\",\"const\":\"afb-event\"},\""
    "event\":{\"type\":\"string\"},\"data\":{\"type\":\"object\"}}}},\"x-perm"
    "issions\":{\"monitor\":{\"permission\":\"urn:AGL:permission:low-can:publ"
    "ic:monitor\"},\"write\":{\"permission\":\"urn:AGL:permission::platform:c"
    "an:write \"}},\"responses\":{\"200\":{\"description\":\"A complex object"
    " array response\",\"content\":{\"application/json\":{\"schema\":{\"$ref\""
    ":\"#/components/schemas/afb-reply\"}}}}}},\"paths\":{\"/subscribe\":{\"d"
    "escription\":\"Subscribe to CAN signals events\",\"parameters\":[{\"in\""
    ":\"query\",\"name\":\"event\",\"required\":false,\"schema\":{\"type\":\""
    "string\"}}],\"responses\":{\"200\":{\"$ref\":\"#/components/responses/20"
    "0\"}}},\"/unsubscribe\":{\"description\":\"Unsubscribe previously suscri"
    "bed signals.\",\"parameters\":[{\"in\":\"query\",\"name\":\"event\",\"re"
    "quired\":false,\"schema\":{\"type\":\"string\"}}],\"responses\":{\"200\""
    ":{\"$ref\":\"#/components/responses/200\"}}},\"/get\":{\"description\":\""
    "get a current value of CAN message\",\"parameters\":[{\"in\":\"query\",\""
    "name\":\"event\",\"required\":false,\"schema\":{\"type\":\"string\"}}],\""
    "responses\":{\"200\":{\"$ref\":\"#/components/responses/200\"}}},\"/list"
    "\":{\"description\":\"get a supported CAN message list\",\"responses\":{"
    "\"200\":{\"$ref\":\"#/components/responses/200\"}}},\"/auth\":{\"descrip"
    "tion\":\"Authenticate session to be raise Level Of Assurance.\",\"get\":"
    "{\"x-permissions\":{\"$ref\":\"#/components/x-permissions/write\"},\"res"
    "ponses\":{\"200\":{\"$ref\":\"#/components/responses/200\"}}}},\"/write\""
    ":{\"description\":\"Write a CAN messages to the CAN bus.\",\"get\":{\"x-"
    "permissions\":{\"LOA\":1},\"parameters\":[{\"in\":\"query\",\"name\":\"b"
    "us_name\",\"required\":false,\"schema\":{\"type\":\"string\"}},{\"in\":\""
    "query\",\"name\":\"frame\",\"required\":false,\"schema\":{\"type\":\"arr"
    "ay\"},\"parameters\":[{\"in\":\"query\",\"name\":\"can_id\",\"required\""
    ":false,\"schema\":{\"type\":\"string\"}},{\"in\":\"query\",\"name\":\"ca"
    "n_dlc\",\"required\":false,\"schema\":{\"type\":\"string\"}},{\"in\":\"q"
    "uery\",\"name\":\"can_data\",\"required\":false,\"schema\":{\"type\":\"s"
    "tring\"}}]},{\"in\":\"query\",\"name\":\"signal_name\",\"required\":fals"
    "e,\"schema\":{\"type\":\"string\"}},{\"in\":\"query\",\"name\":\"signal_"
    "value\",\"required\":false,\"schema\":{\"type\":\"integer\"}}],\"respons"
    "es\":{\"200\":{\"$ref\":\"#/components/responses/200\"}}}}}}"
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

