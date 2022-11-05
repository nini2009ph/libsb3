#ifndef BLOCKS_HPP
#define BLOCKS_HPP

#define EVENT_WHENFLAGCLICKED "event_whenflagclicked"

#define MOTION_GOTOXY "motion_gotoxy"
#define MOTION_GOTOXY_IN(X, Y) {{"X", {1, {4, std::to_string(X)}}}, {"Y", {1, {4, std::to_string(Y)}}}}

#define CONTROL_FOREVER "control_forever" 
#define CONTROL_FOREVER_IN(SUBSTACK) {{"SUBSTACK", {2, SUBSTACK}}}

#define CONTROL_IF "control_if"
#define CONTROL_IF_IN(SUBSTACK, CONDITION) {{"SUBSTACK", {2, SUBSTACK}}, {"CONDITION", {2, CONDITION}}}

#define OPERATOR_EQUALS "operator_equals"
#define OPERATOR_EQUALS_IN(OPERAND1, OPERAND2) {{"OPERAND1", {1, {10, OPERAND1}}}, {"OPERAND2", {1, {10, OPERAND2}}}}

#endif