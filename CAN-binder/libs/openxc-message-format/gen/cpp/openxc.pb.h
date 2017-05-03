/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.1 at Wed Sep 14 16:47:53 2016. */

#ifndef PB_OPENXC_PB_H_INCLUDED
#define PB_OPENXC_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _openxc_VehicleMessage_Type {
    openxc_VehicleMessage_Type_CAN = 1,
    openxc_VehicleMessage_Type_SIMPLE = 2,
    openxc_VehicleMessage_Type_DIAGNOSTIC = 3,
    openxc_VehicleMessage_Type_CONTROL_COMMAND = 4,
    openxc_VehicleMessage_Type_COMMAND_RESPONSE = 5
} openxc_VehicleMessage_Type;

typedef enum _openxc_CanMessage_FrameFormat {
    openxc_CanMessage_FrameFormat_STANDARD = 1,
    openxc_CanMessage_FrameFormat_EXTENDED = 2
} openxc_CanMessage_FrameFormat;

typedef enum _openxc_ControlCommand_Type {
    openxc_ControlCommand_Type_VERSION = 1,
    openxc_ControlCommand_Type_DEVICE_ID = 2,
    openxc_ControlCommand_Type_DIAGNOSTIC = 3,
    openxc_ControlCommand_Type_PASSTHROUGH = 4,
    openxc_ControlCommand_Type_ACCEPTANCE_FILTER_BYPASS = 5,
    openxc_ControlCommand_Type_PAYLOAD_FORMAT = 6,
    openxc_ControlCommand_Type_PREDEFINED_OBD2_REQUESTS = 7,
    openxc_ControlCommand_Type_MODEM_CONFIGURATION = 8,
    openxc_ControlCommand_Type_RTC_CONFIGURATION = 9,
    openxc_ControlCommand_Type_SD_MOUNT_STATUS = 10,
    openxc_ControlCommand_Type_PLATFORM = 11
} openxc_ControlCommand_Type;

typedef enum _openxc_DiagnosticControlCommand_Action {
    openxc_DiagnosticControlCommand_Action_ADD = 1,
    openxc_DiagnosticControlCommand_Action_CANCEL = 2
} openxc_DiagnosticControlCommand_Action;

typedef enum _openxc_PayloadFormatCommand_PayloadFormat {
    openxc_PayloadFormatCommand_PayloadFormat_JSON = 1,
    openxc_PayloadFormatCommand_PayloadFormat_PROTOBUF = 2,
    openxc_PayloadFormatCommand_PayloadFormat_MESSAGEPACK = 3
} openxc_PayloadFormatCommand_PayloadFormat;

typedef enum _openxc_NetworkOperatorSettings_OperatorSelectMode {
    openxc_NetworkOperatorSettings_OperatorSelectMode_AUTOMATIC = 0,
    openxc_NetworkOperatorSettings_OperatorSelectMode_MANUAL = 1,
    openxc_NetworkOperatorSettings_OperatorSelectMode_DEREGISTER = 2,
    openxc_NetworkOperatorSettings_OperatorSelectMode_SET_ONLY = 3,
    openxc_NetworkOperatorSettings_OperatorSelectMode_MANUAL_AUTOMATIC = 4
} openxc_NetworkOperatorSettings_OperatorSelectMode;

typedef enum _openxc_NetworkOperatorSettings_NetworkDescriptor_NetworkType {
    openxc_NetworkOperatorSettings_NetworkDescriptor_NetworkType_GSM = 0,
    openxc_NetworkOperatorSettings_NetworkDescriptor_NetworkType_UTRAN = 2
} openxc_NetworkOperatorSettings_NetworkDescriptor_NetworkType;

typedef enum _openxc_DiagnosticRequest_DecodedType {
    openxc_DiagnosticRequest_DecodedType_NONE = 1,
    openxc_DiagnosticRequest_DecodedType_OBD2 = 2
} openxc_DiagnosticRequest_DecodedType;

typedef enum _openxc_DynamicField_Type {
    openxc_DynamicField_Type_STRING = 1,
    openxc_DynamicField_Type_NUM = 2,
    openxc_DynamicField_Type_BOOL = 3
} openxc_DynamicField_Type;

/* Struct definitions */
typedef struct _openxc_NetworkDataSettings {
    pb_callback_t APN;
} openxc_NetworkDataSettings;

typedef struct _openxc_AcceptanceFilterBypassCommand {
    bool has_bus;
    int32_t bus;
    bool has_bypass;
    bool bypass;
} openxc_AcceptanceFilterBypassCommand;

typedef PB_BYTES_ARRAY_T(8) openxc_CanMessage_data_t;

typedef struct _openxc_CanMessage {
    bool has_bus;
    int32_t bus;
    bool has_id;
    uint32_t id;
    bool has_data;
    openxc_CanMessage_data_t data;
    bool has_frame_format;
    openxc_CanMessage_FrameFormat frame_format;
} openxc_CanMessage;

typedef struct _openxc_CommandResponse {
    bool has_type;
    openxc_ControlCommand_Type type;
    bool has_message;
    char message[128];
    bool has_status;
    bool status;
} openxc_CommandResponse;

typedef PB_BYTES_ARRAY_T(8) openxc_DiagnosticRequest_payload_t;

typedef struct _openxc_DiagnosticRequest {
    bool has_bus;
    int32_t bus;
    bool has_message_id;
    uint32_t message_id;
    bool has_mode;
    uint32_t mode;
    bool has_pid;
    uint32_t pid;
    bool has_payload;
    openxc_DiagnosticRequest_payload_t payload;
    bool has_multiple_responses;
    bool multiple_responses;
    bool has_frequency;
    double frequency;
    bool has_name;
    char name[10];
    bool has_decoded_type;
    openxc_DiagnosticRequest_DecodedType decoded_type;
} openxc_DiagnosticRequest;

typedef PB_BYTES_ARRAY_T(255) openxc_DiagnosticResponse_payload_t;

typedef struct _openxc_DiagnosticResponse {
    bool has_bus;
    int32_t bus;
    bool has_message_id;
    uint32_t message_id;
    bool has_mode;
    uint32_t mode;
    bool has_pid;
    uint32_t pid;
    bool has_success;
    bool success;
    bool has_negative_response_code;
    uint32_t negative_response_code;
    bool has_payload;
    openxc_DiagnosticResponse_payload_t payload;
    bool has_value;
    double value;
} openxc_DiagnosticResponse;

typedef struct _openxc_DynamicField {
    bool has_type;
    openxc_DynamicField_Type type;
    bool has_string_value;
    char string_value[100];
    bool has_numeric_value;
    double numeric_value;
    bool has_boolean_value;
    bool boolean_value;
} openxc_DynamicField;

typedef struct _openxc_NetworkOperatorSettings_NetworkDescriptor {
    bool has_PLMN;
    uint32_t PLMN;
    bool has_networkType;
    openxc_NetworkOperatorSettings_NetworkDescriptor_NetworkType networkType;
} openxc_NetworkOperatorSettings_NetworkDescriptor;

typedef struct _openxc_PassthroughModeControlCommand {
    bool has_bus;
    int32_t bus;
    bool has_enabled;
    bool enabled;
} openxc_PassthroughModeControlCommand;

typedef struct _openxc_PayloadFormatCommand {
    bool has_format;
    openxc_PayloadFormatCommand_PayloadFormat format;
} openxc_PayloadFormatCommand;

typedef struct _openxc_PredefinedObd2RequestsCommand {
    bool has_enabled;
    bool enabled;
} openxc_PredefinedObd2RequestsCommand;

typedef struct _openxc_RTCConfigurationCommand {
    bool has_unix_time;
    uint32_t unix_time;
} openxc_RTCConfigurationCommand;

typedef struct _openxc_ServerConnectSettings {
    bool has_host;
    char host[128];
    bool has_port;
    uint32_t port;
} openxc_ServerConnectSettings;

typedef struct _openxc_DiagnosticControlCommand {
    bool has_request;
    openxc_DiagnosticRequest request;
    bool has_action;
    openxc_DiagnosticControlCommand_Action action;
} openxc_DiagnosticControlCommand;

typedef struct _openxc_NetworkOperatorSettings {
    bool has_allowDataRoaming;
    bool allowDataRoaming;
    bool has_operatorSelectMode;
    openxc_NetworkOperatorSettings_OperatorSelectMode operatorSelectMode;
    bool has_networkDescriptor;
    openxc_NetworkOperatorSettings_NetworkDescriptor networkDescriptor;
} openxc_NetworkOperatorSettings;

typedef struct _openxc_SimpleMessage {
    bool has_name;
    char name[100];
    bool has_value;
    openxc_DynamicField value;
    bool has_event;
    openxc_DynamicField event;
} openxc_SimpleMessage;

typedef struct _openxc_ModemConfigurationCommand {
    bool has_networkOperatorSettings;
    openxc_NetworkOperatorSettings networkOperatorSettings;
    bool has_networkDataSettings;
    openxc_NetworkDataSettings networkDataSettings;
    bool has_serverConnectSettings;
    openxc_ServerConnectSettings serverConnectSettings;
} openxc_ModemConfigurationCommand;

typedef struct _openxc_ControlCommand {
    bool has_type;
    openxc_ControlCommand_Type type;
    bool has_diagnostic_request;
    openxc_DiagnosticControlCommand diagnostic_request;
    bool has_passthrough_mode_request;
    openxc_PassthroughModeControlCommand passthrough_mode_request;
    bool has_acceptance_filter_bypass_command;
    openxc_AcceptanceFilterBypassCommand acceptance_filter_bypass_command;
    bool has_payload_format_command;
    openxc_PayloadFormatCommand payload_format_command;
    bool has_predefined_obd2_requests_command;
    openxc_PredefinedObd2RequestsCommand predefined_obd2_requests_command;
    bool has_modem_configuration_command;
    openxc_ModemConfigurationCommand modem_configuration_command;
    bool has_rtc_configuration_command;
    openxc_RTCConfigurationCommand rtc_configuration_command;
} openxc_ControlCommand;

typedef struct _openxc_VehicleMessage {
    bool has_type;
    openxc_VehicleMessage_Type type;
    bool has_can_message;
    openxc_CanMessage can_message;
    bool has_simple_message;
    openxc_SimpleMessage simple_message;
    bool has_diagnostic_response;
    openxc_DiagnosticResponse diagnostic_response;
    bool has_control_command;
    openxc_ControlCommand control_command;
    bool has_command_response;
    openxc_CommandResponse command_response;
    bool has_timestamp;
    uint64_t timestamp;
} openxc_VehicleMessage;

/* Default values for struct fields */

/* Initializer values for message structs */
#define openxc_VehicleMessage_init_default       {false, (openxc_VehicleMessage_Type)0, false, openxc_CanMessage_init_default, false, openxc_SimpleMessage_init_default, false, openxc_DiagnosticResponse_init_default, false, openxc_ControlCommand_init_default, false, openxc_CommandResponse_init_default, false, 0}
#define openxc_CanMessage_init_default           {false, 0, false, 0, false, {0, {0}}, false, (openxc_CanMessage_FrameFormat)0}
#define openxc_ControlCommand_init_default       {false, (openxc_ControlCommand_Type)0, false, openxc_DiagnosticControlCommand_init_default, false, openxc_PassthroughModeControlCommand_init_default, false, openxc_AcceptanceFilterBypassCommand_init_default, false, openxc_PayloadFormatCommand_init_default, false, openxc_PredefinedObd2RequestsCommand_init_default, false, openxc_ModemConfigurationCommand_init_default, false, openxc_RTCConfigurationCommand_init_default}
#define openxc_DiagnosticControlCommand_init_default {false, openxc_DiagnosticRequest_init_default, false, (openxc_DiagnosticControlCommand_Action)0}
#define openxc_PassthroughModeControlCommand_init_default {false, 0, false, 0}
#define openxc_AcceptanceFilterBypassCommand_init_default {false, 0, false, 0}
#define openxc_PayloadFormatCommand_init_default {false, (openxc_PayloadFormatCommand_PayloadFormat)0}
#define openxc_PredefinedObd2RequestsCommand_init_default {false, 0}
#define openxc_NetworkOperatorSettings_init_default {false, 0, false, (openxc_NetworkOperatorSettings_OperatorSelectMode)0, false, openxc_NetworkOperatorSettings_NetworkDescriptor_init_default}
#define openxc_NetworkOperatorSettings_NetworkDescriptor_init_default {false, 0, false, (openxc_NetworkOperatorSettings_NetworkDescriptor_NetworkType)0}
#define openxc_NetworkDataSettings_init_default  {{{NULL}, NULL}}
#define openxc_ServerConnectSettings_init_default {false, "", false, 0}
#define openxc_ModemConfigurationCommand_init_default {false, openxc_NetworkOperatorSettings_init_default, false, openxc_NetworkDataSettings_init_default, false, openxc_ServerConnectSettings_init_default}
#define openxc_RTCConfigurationCommand_init_default {false, 0}
#define openxc_CommandResponse_init_default      {false, (openxc_ControlCommand_Type)0, false, "", false, 0}
#define openxc_DiagnosticRequest_init_default    {false, 0, false, 0, false, 0, false, 0, false, {0, {0}}, false, 0, false, 0, false, "", false, (openxc_DiagnosticRequest_DecodedType)0}
#define openxc_DiagnosticResponse_init_default   {false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, {0, {0}}, false, 0}
#define openxc_DynamicField_init_default         {false, (openxc_DynamicField_Type)0, false, "", false, 0, false, 0}
#define openxc_SimpleMessage_init_default        {false, "", false, openxc_DynamicField_init_default, false, openxc_DynamicField_init_default}
#define openxc_VehicleMessage_init_zero          {false, (openxc_VehicleMessage_Type)0, false, openxc_CanMessage_init_zero, false, openxc_SimpleMessage_init_zero, false, openxc_DiagnosticResponse_init_zero, false, openxc_ControlCommand_init_zero, false, openxc_CommandResponse_init_zero, false, 0}
#define openxc_CanMessage_init_zero              {false, 0, false, 0, false, {0, {0}}, false, (openxc_CanMessage_FrameFormat)0}
#define openxc_ControlCommand_init_zero          {false, (openxc_ControlCommand_Type)0, false, openxc_DiagnosticControlCommand_init_zero, false, openxc_PassthroughModeControlCommand_init_zero, false, openxc_AcceptanceFilterBypassCommand_init_zero, false, openxc_PayloadFormatCommand_init_zero, false, openxc_PredefinedObd2RequestsCommand_init_zero, false, openxc_ModemConfigurationCommand_init_zero, false, openxc_RTCConfigurationCommand_init_zero}
#define openxc_DiagnosticControlCommand_init_zero {false, openxc_DiagnosticRequest_init_zero, false, (openxc_DiagnosticControlCommand_Action)0}
#define openxc_PassthroughModeControlCommand_init_zero {false, 0, false, 0}
#define openxc_AcceptanceFilterBypassCommand_init_zero {false, 0, false, 0}
#define openxc_PayloadFormatCommand_init_zero    {false, (openxc_PayloadFormatCommand_PayloadFormat)0}
#define openxc_PredefinedObd2RequestsCommand_init_zero {false, 0}
#define openxc_NetworkOperatorSettings_init_zero {false, 0, false, (openxc_NetworkOperatorSettings_OperatorSelectMode)0, false, openxc_NetworkOperatorSettings_NetworkDescriptor_init_zero}
#define openxc_NetworkOperatorSettings_NetworkDescriptor_init_zero {false, 0, false, (openxc_NetworkOperatorSettings_NetworkDescriptor_NetworkType)0}
#define openxc_NetworkDataSettings_init_zero     {{{NULL}, NULL}}
#define openxc_ServerConnectSettings_init_zero   {false, "", false, 0}
#define openxc_ModemConfigurationCommand_init_zero {false, openxc_NetworkOperatorSettings_init_zero, false, openxc_NetworkDataSettings_init_zero, false, openxc_ServerConnectSettings_init_zero}
#define openxc_RTCConfigurationCommand_init_zero {false, 0}
#define openxc_CommandResponse_init_zero         {false, (openxc_ControlCommand_Type)0, false, "", false, 0}
#define openxc_DiagnosticRequest_init_zero       {false, 0, false, 0, false, 0, false, 0, false, {0, {0}}, false, 0, false, 0, false, "", false, (openxc_DiagnosticRequest_DecodedType)0}
#define openxc_DiagnosticResponse_init_zero      {false, 0, false, 0, false, 0, false, 0, false, 0, false, 0, false, {0, {0}}, false, 0}
#define openxc_DynamicField_init_zero            {false, (openxc_DynamicField_Type)0, false, "", false, 0, false, 0}
#define openxc_SimpleMessage_init_zero           {false, "", false, openxc_DynamicField_init_zero, false, openxc_DynamicField_init_zero}

/* Field tags (for use in manual encoding/decoding) */
#define openxc_NetworkDataSettings_APN_tag       1
#define openxc_AcceptanceFilterBypassCommand_bus_tag 1
#define openxc_AcceptanceFilterBypassCommand_bypass_tag 2
#define openxc_CanMessage_bus_tag                1
#define openxc_CanMessage_id_tag                 2
#define openxc_CanMessage_data_tag               3
#define openxc_CanMessage_frame_format_tag       4
#define openxc_CommandResponse_type_tag          1
#define openxc_CommandResponse_message_tag       2
#define openxc_CommandResponse_status_tag        3
#define openxc_DiagnosticRequest_bus_tag         1
#define openxc_DiagnosticRequest_message_id_tag  2
#define openxc_DiagnosticRequest_mode_tag        3
#define openxc_DiagnosticRequest_pid_tag         4
#define openxc_DiagnosticRequest_payload_tag     5
#define openxc_DiagnosticRequest_multiple_responses_tag 6
#define openxc_DiagnosticRequest_frequency_tag   7
#define openxc_DiagnosticRequest_name_tag        8
#define openxc_DiagnosticRequest_decoded_type_tag 9
#define openxc_DiagnosticResponse_bus_tag        1
#define openxc_DiagnosticResponse_message_id_tag 2
#define openxc_DiagnosticResponse_mode_tag       3
#define openxc_DiagnosticResponse_pid_tag        4
#define openxc_DiagnosticResponse_success_tag    5
#define openxc_DiagnosticResponse_negative_response_code_tag 6
#define openxc_DiagnosticResponse_payload_tag    7
#define openxc_DiagnosticResponse_value_tag      8
#define openxc_DynamicField_type_tag             1
#define openxc_DynamicField_string_value_tag     2
#define openxc_DynamicField_numeric_value_tag    3
#define openxc_DynamicField_boolean_value_tag    4
#define openxc_NetworkOperatorSettings_NetworkDescriptor_PLMN_tag 1
#define openxc_NetworkOperatorSettings_NetworkDescriptor_networkType_tag 2
#define openxc_PassthroughModeControlCommand_bus_tag 1
#define openxc_PassthroughModeControlCommand_enabled_tag 2
#define openxc_PayloadFormatCommand_format_tag   1
#define openxc_PredefinedObd2RequestsCommand_enabled_tag 1
#define openxc_RTCConfigurationCommand_unix_time_tag 1
#define openxc_ServerConnectSettings_host_tag    1
#define openxc_ServerConnectSettings_port_tag    2
#define openxc_DiagnosticControlCommand_request_tag 1
#define openxc_DiagnosticControlCommand_action_tag 2
#define openxc_NetworkOperatorSettings_allowDataRoaming_tag 1
#define openxc_NetworkOperatorSettings_operatorSelectMode_tag 2
#define openxc_NetworkOperatorSettings_networkDescriptor_tag 3
#define openxc_SimpleMessage_name_tag            1
#define openxc_SimpleMessage_value_tag           2
#define openxc_SimpleMessage_event_tag           3
#define openxc_ModemConfigurationCommand_networkOperatorSettings_tag 1
#define openxc_ModemConfigurationCommand_networkDataSettings_tag 2
#define openxc_ModemConfigurationCommand_serverConnectSettings_tag 3
#define openxc_ControlCommand_type_tag           1
#define openxc_ControlCommand_diagnostic_request_tag 2
#define openxc_ControlCommand_passthrough_mode_request_tag 3
#define openxc_ControlCommand_acceptance_filter_bypass_command_tag 4
#define openxc_ControlCommand_payload_format_command_tag 5
#define openxc_ControlCommand_predefined_obd2_requests_command_tag 6
#define openxc_ControlCommand_modem_configuration_command_tag 7
#define openxc_ControlCommand_rtc_configuration_command_tag 8
#define openxc_VehicleMessage_type_tag           1
#define openxc_VehicleMessage_can_message_tag    2
#define openxc_VehicleMessage_simple_message_tag 3
#define openxc_VehicleMessage_diagnostic_response_tag 4
#define openxc_VehicleMessage_control_command_tag 5
#define openxc_VehicleMessage_command_response_tag 6
#define openxc_VehicleMessage_timestamp_tag      7

/* Struct field encoding specification for nanopb */
extern const pb_field_t openxc_VehicleMessage_fields[8];
extern const pb_field_t openxc_CanMessage_fields[5];
extern const pb_field_t openxc_ControlCommand_fields[9];
extern const pb_field_t openxc_DiagnosticControlCommand_fields[3];
extern const pb_field_t openxc_PassthroughModeControlCommand_fields[3];
extern const pb_field_t openxc_AcceptanceFilterBypassCommand_fields[3];
extern const pb_field_t openxc_PayloadFormatCommand_fields[2];
extern const pb_field_t openxc_PredefinedObd2RequestsCommand_fields[2];
extern const pb_field_t openxc_NetworkOperatorSettings_fields[4];
extern const pb_field_t openxc_NetworkOperatorSettings_NetworkDescriptor_fields[3];
extern const pb_field_t openxc_NetworkDataSettings_fields[2];
extern const pb_field_t openxc_ServerConnectSettings_fields[3];
extern const pb_field_t openxc_ModemConfigurationCommand_fields[4];
extern const pb_field_t openxc_RTCConfigurationCommand_fields[2];
extern const pb_field_t openxc_CommandResponse_fields[4];
extern const pb_field_t openxc_DiagnosticRequest_fields[10];
extern const pb_field_t openxc_DiagnosticResponse_fields[9];
extern const pb_field_t openxc_DynamicField_fields[5];
extern const pb_field_t openxc_SimpleMessage_fields[4];

/* Maximum encoded size of messages (where known) */
#define openxc_CanMessage_size                   33
#define openxc_DiagnosticControlCommand_size     76
#define openxc_PassthroughModeControlCommand_size 13
#define openxc_AcceptanceFilterBypassCommand_size 13
#define openxc_PayloadFormatCommand_size         6
#define openxc_PredefinedObd2RequestsCommand_size 2
#define openxc_NetworkOperatorSettings_size      22
#define openxc_NetworkOperatorSettings_NetworkDescriptor_size 12
#define openxc_ServerConnectSettings_size        137
#define openxc_RTCConfigurationCommand_size      6
#define openxc_CommandResponse_size              139
#define openxc_DiagnosticRequest_size            68
#define openxc_DiagnosticResponse_size           304
#define openxc_DynamicField_size                 119
#define openxc_SimpleMessage_size                344

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif