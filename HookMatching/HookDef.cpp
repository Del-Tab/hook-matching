#include "HookDef.hpp"


//////////////////////////
// Note Hook            //
//////////////////////////
boolean Note::hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth) {
  return hc[depth] == 0;
}

struct note_info Note::getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth) {
  struct note_info ret = { 0, 0, duration};
  ++(hc[depth]);
  return ret;
}

uint8_t Note::getMaxDepth() {
  return 1;
}



//////////////////////////
// Repeat Hook          //
//////////////////////////
RepeatHook::~RepeatHook() {
  p->unuse();
}

boolean RepeatHook::hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth) {
  if (nb_cycles == 0)
    return true;
  if (hc[depth] >= nb_cycles)
    return false;
  return p->hasMore(hc, maxDepth, depth + 1);
}

struct note_info RepeatHook::getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth) {
  struct note_info childOne = p->getOne(hc, maxDepth, depth + 1);
  if (!p->hasMore(hc, maxDepth, depth + 1)) {
    hc[depth + 1] = 0;
    ++(hc[depth]);
  }
  return childOne;
}

uint8_t RepeatHook::getMaxDepth() {
  return p->getMaxDepth() + 1;
}

//////////////////////////
// List Hook            //
//////////////////////////
ListHook::ListHook(uint16_t a_capacity) : capacity(a_capacity), number(0), maxDepth(0) {
  list = (struct PlayableChild *)malloc(a_capacity * sizeof(struct PlayableChild));
  if (list == NULL)
    Serial.println("malloc error");
}
ListHook::~ListHook() {
  if (list != NULL) {
    for (int i = 0; i < number; ++i)
      list[i].p->unuse();
    free(list);
  }
}

ListHook *ListHook::add(Playable *p, int8_t degreeOffset, effects flags) {
  list[number++] = {degreeOffset, flags, p->useAgain()};
  maxDepth = max(maxDepth, p->getMaxDepth() + 1);
  return this;
}
boolean ListHook::hasMore(uint8_t *hc, uint8_t maxDepth, uint8_t depth) {
  if (hc[depth] < number - 1)
    return true;
  if (hc[depth] >= number)
    return false;
  list[hc[depth]].p->hasMore(hc, maxDepth, depth + 1);
}
struct note_info ListHook::getOne(uint8_t *hc, uint8_t maxDepth, uint8_t depth) {
  struct PlayableChild pc = list[hc[depth]];
  note_info ret = pc.p->getOne(hc, maxDepth, depth + 1);
  if (!pc.p->hasMore(hc, maxDepth, depth + 1)) {
    hc[depth + 1] = 0;
    ++(hc[depth]);
  }
  ret.flags |= pc.flags;
  // correction with the current structure's offset :)
  ret.degreeOffset += pc.degreeOffset;
  return ret;
}
uint8_t ListHook::getMaxDepth() {
  return maxDepth;
}
