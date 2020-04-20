#
#
# Last edits:
# ----------------------------------------------------------
# April 20, 2020
# - Added a switch to enable or disable the pose eval renderer. 


SET( WITH_POSEEVAL_RENDERER OFF)
OPTION(WITH_POSEEVAL_RENDERER "Include the PoseEvalRenderer project" OFF)

if(${WITH_POSEEVAL_RENDERER})
add_subdirectory(PoseEvalRenderer)
endif()