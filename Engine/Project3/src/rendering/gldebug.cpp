#include "gldebug.h"
#include <assert.h>

void GLDebugClear(GLDebugContext *ctx)
{
    ctx->col = {1.0, 1.0, 1.0};
    ctx->numLines = 0;
}

void GLDebugSetColor(GLDebugContext *ctx, float r, float g, float b)
{
    ctx->col = {r, g, b};
}

void GLDebugAddLine(GLDebugContext *ctx, float x1, float y1, float z1, float x2, float y2, float z2)
{
    assert(ctx->numLines < GL_DEBUG_CONTEXT_MAX_LINES);
    float r = ctx->col.r;
    float g = ctx->col.g;
    float b = ctx->col.b;
    GLDebugVertex vertex1 = { {x1, y1, z1}, {r, g, b} };
    GLDebugVertex vertex2 = { {x2, y2, z2}, {r, g, b} };
    GLDebugLine line = {vertex1, vertex2};
    ctx->lines[ctx->numLines] = line;
    ctx->numLines++;
}
