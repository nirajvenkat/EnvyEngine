// taskPayload.h
//
// The task payload is the "meat" of a render task and determines what the task consists of.

#ifndef RENDERTASKPAYLOAD_H
#define RENDERTASKPAYLOAD_H

#define RENDERTASKPAYLOAD_HEADER_SIZE	(sizeof(char) + sizeof(size_t))

// RENDER TASK TYPES
//
// 0 - Render From View (use view matrix). Task data consists of a single glm::mat4.

typedef struct RenderTaskPayload {
	size_t	totalSize;	 // Total size of task data plus header size.
	size_t  dataSize;    // Should be equal to totalSize - RENDERTASKPAYLOAD_HEADER_SIZE
	char	type;		 // Task type. Make larger if we later find we need more than 256 tasks (unlikely)
	char	_padding[3]; // Unused
	char	taskData[1]; // Task data of variable length
} RenderTaskPayload;

#endif
