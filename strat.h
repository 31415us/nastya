/** @file strat.h
 * @brief Strategy file
 *
 * This file implements all functions needed to give the "intelligence" of the
 * robot, also known as the strategy.
 *
 * Our strategy's graph can be seen in the following picture :
 * \dotfile doc/strat.dot "Strategy for 2013"
 *
 * The difference between playing in red and playing in blue is a change in the
 * coordinate system : The starting corner (away from the gift) is always at (0, 0)
 * and the X axis points toward the long side of the table. Therefore, since we want
 * our coordinate system to remain direct, the Y axis changes direction. When we play
 * as the red team, the Y axis points in the table and when we play on the blue side,
 * the Y axis points \a outside the table.
 *
 * To avoid error, when you use coordinate and/or angles, you should \a always use
 * the COLOR_Y and COLOR_A macros to specify a coordinate. Those macros will
 * do the correct coordinate change based on the color of the robot.
 */
#ifndef _STRAT_H_
#define _STRAT_H_

#include <aversive.h>
#include <vect_base.h>

/** Duration of a match in seconds. */
#define MATCH_TIME 89

/* Return values for trajectories. */
#define END_TRAJ       1 /**< Trajectory successful. */
#define END_BLOCKING   2 /**< Blocking during trajectory. */
#define END_NEAR       4 /**< Arrived near point. */
#define END_OBSTACLE   8 /**< There is an obstacle in front of us */
#define END_ERROR     16 /**< Cannot do the command */
#define END_TIMER     32 /**< End of match timer. */

/** Checks if an return code indicates a succesful trajectory. */
#define TRAJ_SUCCESS(f) ((f) & (END_TRAJ|END_NEAR))

/** Flags for "standard" trajectories. */
#define TRAJ_FLAGS_STD (END_TRAJ|END_BLOCKING|END_OBSTACLE|END_TIMER|END_ERROR)

/** Flags for "cutting corners" trajectories.
 * @warning Using this type of trajectories lowers the precision.
 */
#define TRAJ_FLAGS_NEAR (TRAJ_FLAGS_STD|END_NEAR)

/** This enum is used for specifying a team color. */
typedef enum {RED, BLUE} strat_color_t;

/** Computes the symmetrical position depending on color. */ 
#define COLOR_Y(x) (strat.color == RED ? (x) : 2000 - (x))

/** Computes the symmetrical angle depending on color. */
#define COLOR_A(x) (strat.color == RED ? (x) : -(x))

/** Computes correctional value for the servo position */
#define COLOR_C (strat.color == BLUE ? (20) : -(20))

/**
 * @brief A glass on the table.
 *
 * This structure holds every information related to the glass on the table.
 */
typedef struct {
    point_t pos; /**< Position of the glass on the playing field. */
    int taken; /**< =1 if this glass was already taken. */
} glass_t;

/** 
 * A gift on the side of the table.
 */
typedef struct {
    int done; /**< =1 if this gift is down. */
    int x;    /**< X coordinate of this gift. */
    int last_try_time; /**< Last time we tried to do this gift, in s since match start. */
} gift_t;

/** This structure holds all the configuration data and state of the strategy. */
struct strat_info {
    strat_color_t color;                /**< Color of our robot. */

    /** @brief The glasses on the playing field.
     * \image html doc/glasses_position.png "Indexes of the glasses for the red team."
     *
     * The glass 0 and 1 are sometimes called the "outer glasses" in the code 
     * while the 3 and 4 are called the "inner glasses". 
     *
     * When playing in blue, the indexes are simply mirrored, so the glasses
     * closer to the blue starting zone become glasses 0 and 1.
     */
    glass_t glasses[12];
    /** @brief The Cakes on the playing field.
     * \image html doc/gifts_position.png "Indexes of the gifts."
     */
    gift_t gifts[4];
    
    /** Save the state for the strategical finite state machine */
    int state; /** Currently the gift we are working one  (in the future)*/
    int sub_state;
    int avoiding;

    int time; /**< Time since the beginning of the match, in seconds. */

    /* Configuration flags. */
    /** =1 If we should take the 1st glass on the left side, 0 if we take it on the right.*/
    int take_1st_glass_left;
};

/** This global var holds everything related to the strat. */
extern struct strat_info strat;

/** Auto positions the robot before the match. 
 *
 * This function positions the robot using the border as references. The
 * color is assumed to be already configured.
 * 
 * @param [in] x, y The starting coordinates, in mm.
 * @param [in] a The starting angle relative to the X-axis, in degrees. 
 * @param epaisseurRobot The distamce between the back of the robot and the wheel axis. 
 */
void strat_autopos(int16_t x, int16_t y, int16_t a, int16_t epaisseurRobot);

/** Tests for end of trajectory.
 *
 * @param [in] why The allowed reasons for this function to return true.
 * @returns An error code indicating the reason of the end of the trajectory.
 */
int test_traj_end(int why);

/** Waits for the end of a trajectory.
 *
 * @param [in] why The allowed reasons to end the trajectory.
 * @returns An error code indicating the reason of the end of the trajectory.
 */
int wait_traj_end(int why);

/** @brief Inits the object positions in the strat_info_t structure.
 * @note This function supposes the color has \a already been set.
 * @sa strat_info
 */
void strat_set_objects(void);

/** @brief Starts a match
 *
 * This function starts the match. It will \a not check for the starting cord
 * so the caller should do it.
 */
void strat_begin(strat_color_t color);



void strat_do_gift(int number);
void strat_long_arm_up(void);
void strat_long_arm_down(void);
void strat_short_arm_up(void);
void strat_short_arm_down(void);

void strat_avoiding(void);
void strat_restart_after_avoiding(void);

#endif
