/**
 * @file main.c
 * @author Nicolas IRAGNE (nicoragne@hotmail.fr)
 * @brief GPGGA NMEA-0183 demo
 * This file is a quick and dirty implementation of a GPGGA NMEA-0183 parser.
 * It is not meant to be functional, but rather was a little fun C exercise.
 * TODO: Implement other NMEA-0183 messages
 * TODO: Compute checksum in a generic way, so even unsupported messages can
 * checked
 * @date 2023-06-01
 * @copyright Copyright niragne (c) 2023
 */

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS // shut up bill gates
#endif // _WIN32

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum message_type_s
{
    UNKNOWN,
    GGA,
    GLL,
    GSA,
    GSV,
    RMC,
    __MAX_MESSAGE_TYPE,
} message_type_t;

#define NMEA_IDENTIFIER "$GP"
#define GGA_IDENTIFIER "GGA"
#define GLL_IDENTIFIER "GLL"
#define GSA_IDENTIFIER "GSA"
#define GSV_IDENTIFIER "GSV"
#define RMC_IDENTIFIER "RMC"
#define NMEA_MESSAGE_MAX_LENGTH 82
#define NMEA_DELIMITER ","
#define NMEA_CHECKSUM_DELIMITER "*"

// error codes
#define NMEA_SUCCESS 0
#define NMEA_ERROR_INVALID_MESSAGE_TYPE -1
#define NMEA_ERROR_INVALID_CHECKSUM -2
#define NMEA_ERROR_MALFORMED_MESSAGE -3
#define NMEA_ERROR_INTERNAL -4
#define NMEA_ERROR_UNSUPPORTED_MESSAGE_TYPE -5
#define NMEA_ERROR_INVALID_IDENTIFIER -6

// ugly map because C is stupid
static const char* message_type_to_string[] = {
    [UNKNOWN] = "UNKNOWN",
    [GGA] = "GGA",
    [GLL] = "GLL",
    [GSA] = "GSA",
    [GSV] = "GSV",
    [RMC] = "RMC",
    [__MAX_MESSAGE_TYPE] = "MAX_MESSAGE_TYPE -- THIS SHOULD NEVER HAPPEN WTF ARE YOU DOING"
};

// same one for GPS fix quality
static const char* fix_quality_to_string[] = {
    [0] = "Invalid",
    [1] = "GPS",
    [2] = "DGPS",
    [3] = "GPS PPS",
    [4] = "RTK fixed",
    [5] = "RTK float",
    [6] = "Estimated",
    [7] = "Manual",
    [8] = "Simulated",
    [9] = "MAX_FIX_QUALITY -- THIS SHOULD NEVER HAPPEN WTF ARE YOU DOING"
};

typedef enum fix_quality_s
{
    INVALID = 0,
    GPS_FIX = 1,
    DGPS_FIX = 2,
    PPS_FIX = 3,
    RTK = 4,
    FLOAT_RTK = 5,
    ESTIMATED = 6,
    MANUAL = 7,
    SIMULATION = 8
} fix_quality_t;

typedef struct gga_message_s
{
    time_t utc_time;
    double latitude;
    double longitude;
    fix_quality_t fix_quality;
    uint8_t satellites_in_use;
    double hdop;
    double altitude;
    char altitude_units;
    double geoidal_separation;
    char geoidal_separation_units;
    uint32_t dgps_age;
    uint32_t dgps_station_id;
} gga_message_t;

typedef struct nmea_message_s
{
    message_type_t type;
    union
    {
        gga_message_t gga;
    };
    uint8_t checksum;
    uint8_t computed_checksum;
} nmea_message_t;

void print_coordinates(float lat, float lon)
{
    lat = fabsf(lat);
    lon = fabsf(lon);
    int lat_degrees = (int)lat / 100;
    int lon_degrees = (int)lon / 100;
    int lat_minutes = fabsf(lat) - (lat_degrees * 100);
    int lon_minutes = fabsf(lon) - (lon_degrees * 100);

    float lat_seconds = (fabsf(lat) - (lat_degrees * 100) - lat_minutes) * 60;
    float lon_seconds = (fabsf(lon) - (lon_degrees * 100) - lon_minutes) * 60;

    char lat_hemisphere = (lat < 0) ? 'S' : 'N';
    char lon_hemisphere = (lon < 0) ? 'W' : 'E';

    wchar_t degrees_symbol = 0xF8; // °
    printf("%d%c %d' %.2f\" %c, %d%c %d' %.2f\" %c", lat_degrees, degrees_symbol, lat_minutes, lat_seconds, lat_hemisphere, lon_degrees, degrees_symbol, lon_minutes, lon_seconds, lon_hemisphere);
}

void print_gga(const nmea_message_t* msg)
{
    char time_buffer[9];
    strftime(time_buffer, 9, "%H:%M:%S", gmtime(&msg->gga.utc_time));
    printf("\t\tCoordinates:  ");
    print_coordinates(msg->gga.latitude, msg->gga.longitude);
    printf(" (Quality: %s)\n", fix_quality_to_string[msg->gga.fix_quality]);
    printf("\t\tUTC time: %s", time_buffer);
    printf("\tSatellites in use: %d\t", msg->gga.satellites_in_use);
    printf("\tHDOP: %.2f\n", msg->gga.hdop);
    printf("\t\tAltitude: %.2f %c", msg->gga.altitude, msg->gga.altitude_units);
    printf("\tGeoidal separation: %.2f %c\n", msg->gga.geoidal_separation, msg->gga.geoidal_separation_units);
    printf("\t\tDGPS age: %d", msg->gga.dgps_age);
    printf("\t\tDGPS station ID: %d\n", msg->gga.dgps_station_id);
}

void print_gll(const nmea_message_t* msg)
{
    printf("\t\tUNIMPLEMENTED\n");
}

void print_gsa(const nmea_message_t* msg)
{
    printf("\t\tUNIMPLEMENTED\n");
}

void print_gsv(const nmea_message_t* msg)
{
    printf("\t\tUNIMPLEMENTED\n");
}

void print_rmc(const nmea_message_t* msg)
{
    printf("\t\tUNIMPLEMENTED\n");
}

/**
 * @brief ChatGPT provided drop-in replacement for strtok that takes multiple delimiters into account
 */
char* my_strtok(char** string, char* delimiter)
{
    if (*string == NULL)
    {
        return NULL;
    }

    char* start = *string;
    char* end = strstr(start, delimiter);

    if (end == NULL)
    {
        *string = NULL;
        return start;
    }

    *end = '\0';
    *string = end + strlen(delimiter);

    return start;
}

int parse_gga(char* payload, nmea_message_t* msg)
{
    // gga messages are always the same length
    // format:
    // 092751.000,5321.6802,N,00630.3371,W,1,8,1.03,61.7,M,55.3,M,,*75
    char* tokens[14];
    char* token = my_strtok(&payload, NMEA_DELIMITER);
    int i = 0;
    while (token != NULL)
    {
        tokens[i] = token;
        token = my_strtok(&payload, NMEA_DELIMITER);
        i++;
    }
    if (i != 14)
    {
        printf("\t[-] Expected 14 tokens, got %d\n", i);
        return NMEA_ERROR_MALFORMED_MESSAGE;
    }
    // find * in last token
    char* checksum = strstr(tokens[i - 1], NMEA_CHECKSUM_DELIMITER);
    if (checksum == NULL)
    {
        printf("\t[-] Checksum delimiter not found\n");
        return NMEA_ERROR_MALFORMED_MESSAGE;
    }
    *checksum = '\0';
    checksum++;

    // parse tokens
    msg->gga.utc_time = atoi(tokens[0]);
    msg->gga.latitude = atof(tokens[1]);
    if (tokens[2][0] == 'S')
        msg->gga.latitude *= -1;
    msg->gga.longitude = atof(tokens[3]);
    if (tokens[4][0] == 'W')
        msg->gga.longitude *= -1;
    msg->gga.fix_quality = atoi(tokens[5]);
    msg->gga.satellites_in_use = atoi(tokens[6]);
    msg->gga.hdop = atof(tokens[7]);
    msg->gga.altitude = atof(tokens[8]);
    msg->gga.altitude_units = tokens[9][0];
    msg->gga.geoidal_separation = atof(tokens[10]);
    msg->gga.geoidal_separation_units = tokens[11][0];
    msg->gga.dgps_age = atoi(tokens[12]);
    msg->gga.dgps_station_id = atoi(tokens[13]);
    msg->checksum = strtol(checksum, NULL, 16);
    if (msg->checksum != msg->computed_checksum)
    {
        printf("\t[-] Checksum mismatch (expected %d, got %d)\n", msg->computed_checksum, msg->checksum);
        // uncomment to fail on checksum mismatch; left for testing
        // return NMEA_ERROR_INVALID_CHECKSUM;
    }
    return 0;
}

int parse_gll(char* payload, nmea_message_t* msg)
{
    return NMEA_ERROR_UNSUPPORTED_MESSAGE_TYPE;
}

int parse_gsa(char* payload, nmea_message_t* msg)
{
    return NMEA_ERROR_UNSUPPORTED_MESSAGE_TYPE;
}

int parse_gsv(char* payload, nmea_message_t* msg)
{
    return NMEA_ERROR_UNSUPPORTED_MESSAGE_TYPE;
}

int parse_rmc(char* payload, nmea_message_t* msg)
{
    return NMEA_ERROR_UNSUPPORTED_MESSAGE_TYPE;
}

// global fn array cause im lazy
void (*print_message_fn[])(const nmea_message_t*) = {
    [GGA] = print_gga,
    [GLL] = print_gll,
    [GSA] = print_gsa,
    [GSV] = print_gsv,
    [RMC] = print_rmc
};

// same one for parsing, ya know
int (*parse_message_fn[])(char*, nmea_message_t*) = {
    [GGA] = parse_gga,
    [GLL] = parse_gll,
    [GSA] = parse_gsa,
    [GSV] = parse_gsv,
    [RMC] = parse_rmc
};

void print_message(const nmea_message_t* msg)
{
    if (msg->type == UNKNOWN)
    {
        printf("\t[-] Unable to print message: unknown message type %d\n", msg->type);
        return;
    }
    // print common fields
    printf("\t===== MESSAGE START =====\n");
    printf("\tNMEA\n");
    printf("\t\tType: %s\tChecksum: 0x%02x (computed 0x%02x)\n", message_type_to_string[msg->type], msg->checksum, msg->computed_checksum);
    printf("\t%s\n", message_type_to_string[msg->type]);
    print_message_fn[msg->type](msg);
    printf("\t===== MESSAGE END =====\n");
}

/**
 * @brief Get the message type from the NMEA identifier.
 * @param identifier This should be the NMEA identifier, e.g. GGA.
 * @return message_type_t The message type, or __MAX_MESSAGE_TYPE if the identifier is invalid.
 */
message_type_t get_message_type(const char* identifier)
{
    if (strncmp(identifier, "GGA", 3) == 0)
        return GGA;
    else if (strncmp(identifier, "GLL", 3) == 0)
        return GLL;
    else if (strncmp(identifier, "GSA", 3) == 0)
        return GSA;
    else if (strncmp(identifier, "GSV", 3) == 0)
        return GSV;
    else if (strncmp(identifier, "RMC", 3) == 0)
        return RMC;
    return UNKNOWN;
}

uint8_t compute_checksum(const char* s)
{
    // XOR all the characters between s + 1 and the *
    uint8_t checksum = 0;
    const char* end = strstr(s, NMEA_CHECKSUM_DELIMITER);
    if (end == NULL)
        return 0;
    for (const char* c = s + 1; c < end; c++)
    {
        checksum ^= *c;
    }
    return checksum;
}

/**
 * @brief Parse a NMEA message.
 * @param payload The NMEA message payload.
 * @param msg The message to populate.
 * @return int 0 on success, an error code otherwise.
 */
int parse_message(const char* payload_original, nmea_message_t* msg)
{
    // make a copy of the payload so we can modify it
    char payload[NMEA_MESSAGE_MAX_LENGTH + 1];
    memset(payload, 0, sizeof(payload));
    memcpy(payload, payload_original, strlen(payload_original) + 1);
    memset(msg, 0, sizeof(nmea_message_t));
    printf("[+] Parsing message: <%s>\n", payload);
    // try to avoid going through the whole message multiple times if possible
    // we'll use this to keep track of the current token
    char* token = payload;

    // check for the NMEA identifier
    if (strncmp(token, NMEA_IDENTIFIER, strlen(NMEA_IDENTIFIER)) != 0)
    {
        printf("\t[-] Invalid NMEA identifier\n");
        return NMEA_ERROR_INVALID_IDENTIFIER;
    }
    token += strlen(NMEA_IDENTIFIER);

    msg->type = get_message_type(token);
    printf("\t[+] Detected message type %s\n", message_type_to_string[msg->type]);
    if (msg->type == UNKNOWN)
    {
        printf("\t[-] Unknown message type\n");
        return NMEA_ERROR_INVALID_MESSAGE_TYPE;
    }
    token += 4; // let's be nice and skip the comma
    msg->computed_checksum = compute_checksum(payload_original);
    // parse the rest of the message
    return parse_message_fn[msg->type](token, msg);
}

int main()
{
    // path to file containing multiple NMEA messages
    const char* filename = "../sample.txt";
    FILE* f = fopen(filename, "r");
    int err = 0;
    if (f == NULL)
    {
        printf("[-] Failed to open file %s\n", filename);
        err = -1;
        goto _end;
    }
    // try to interpret each line as a NMEA message, line by line
    char line[NMEA_MESSAGE_MAX_LENGTH + 1];
    while (fgets(line, NMEA_MESSAGE_MAX_LENGTH + 1, f) != NULL)
    {
        nmea_message_t msg;
        // remove the newline character
        line[strlen(line) - 1] = '\0';
        int ret = parse_message(line, &msg);
        if (ret != NMEA_SUCCESS && ret != NMEA_ERROR_UNSUPPORTED_MESSAGE_TYPE)
        {
            printf("[-] Failed to parse message\n");
            err = -1;
            goto _end;
        }
        print_message(&msg);
    }
_end:
    if (f != NULL)
        fclose(f);
    return err;
}