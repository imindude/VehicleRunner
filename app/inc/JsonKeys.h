/**
 * @subpage     JSON Config Key String
 * @author      imindude@gmail.com
 *
 * @code
 *               "MAVLink URL": "udp://:14550",
 *               "MAV Log w/o SD": false,
 *               "MAV Plan w/o SD": false,
 *               "Plugin List": [
 *                   "SmallSwarm"
 *               ],
 *               "SmallSwarm Section": {
 *                   "Leader": true,
 *                   "Sub-Leader": false,
 *                   "Member No": 1,
 *                   "Leader URL": "tcp://172.24.200.10:9872",
 *                   "Sub-Leader URL": "tcp://172.24.200.110:9872",
 *                   "Member 1": "tcp://172.24.200.111:9872",
 *                   "Member 2": "tcp://172.24.200.112:9872",
 *                   "Member 3": "tcp://172.24.200.113:9872",
 *                   "Plugin Name": "SmallSwarm"
 *               },
 * @endcode
 */

#define JSON_KEY_MAVLINK_URL    "MAVLink URL"
#define JSON_KEY_MAV_LOG_WO_SD  "MAV Log w/o SD"
#define JSON_KEY_MAV_PLAN_WO_SD "MAV Plan w/o SD"
#define JSON_KEY_PLUGIN_LIST    "Plugin List"

#define JSON_KEY_PLUGIN_SMALL_SWARM         "SmallSwarm"
#define JSON_KEY_SMALL_SWARM_SECTION        "SmallSwarm Section"
#define JSON_KEY_SMALL_SWARM_LEADER         "Leader"
#define JSON_KEY_SMALL_SWARM_SUB_LEADER     "Sub-Leader"
#define JSON_KEY_SMALL_SWARM_MEMBER_NO      "Member No"
#define JSON_KEY_SMALL_SWARM_LEADER_URL     "Leader URL"
#define JSON_KEY_SMALL_SWARM_SUB_LEADER_URL "Sub-Leader URL"
#define JSON_KEY_SMALL_SWARM_MEMBER_1       "Member 1"
#define JSON_KEY_SMALL_SWARM_MEMBER_2       "Member 2"
#define JSON_KEY_SMALL_SWARM_MEMBER_3       "Member 3"

#define JSON_KEY_SMALL_SWARM_MEMBER_PARTIAL "Member "
