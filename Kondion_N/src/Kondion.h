/*
 * KondionWindow.h
 *
 *  Created on: Aug 22, 2016
 *      Author: neal
 */

#ifndef KONDION_H_
#define KONDION_H_

// or Bristlecone
#define KONDION_VERSION "0.1.0 Bayberry"

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <istream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

// TODO: eventually replace a bunch of uint16_t with 32_t because of object count

namespace Kondion {

class KObj_Node;
class KObj_Oriented;
class KObj_Renderable;
class KObj_Entity;
class KObj_Instance;

namespace Physics {
class CollisionInfo;
}

namespace KObj {
class GKO_World;
}

class KComponent;
class KMaterial;

// Base kobj classes

/** Base class for all KObjs, only a node in the tree.
 *  type: 0
 */
class KObj_Node {
 public:
  // All of the KObj_Nodes in existance, including the ones that aren't in world.
  static std::vector<KObj_Node *> all;
  // The current world object, has no parent
  static KObj::GKO_World* worldObject;
  // what is this for?
  static const std::string myClass;
  // Name to identify with
  std::string name;
  // All the children
  std::vector<KObj_Node*> children;
  // How deep the object is in the tree. World is 0, object in world is 1.
  uint8_t depth;
  // Index out of all of the KObj_Nodes in existance
  uint16_t allIndex;
  // My index out of all of siblings
  uint16_t myIndex;
  // Index of the object up the parent chain, usually the world
  uint16_t topObject;
  // How large the tree is, this as root. (all of children's children...)
  uint16_t treeSize;
  // 32 bits of layers, use bitwise
  uint32_t drawLayer;
  // The JS object that is bound
  void* jsObject;
  virtual int getType() {
    return 0;
  }
  void setParent(KObj_Node* node);
  KObj_Node* getParent();
  virtual void updateA() {
  }
  virtual void updateB() {
  }
  virtual const std::string* getClass() {
    return &myClass;
  }
  KObj_Node();
  virtual ~KObj_Node() {
  }
 protected:
  KObj_Node* parent = NULL;
};

// GKO, type 1

/** Base class for Entity and Instance, has an orientation in the world.
 *  type: 2
 */
class KObj_Oriented : public KObj_Node {
 public:
  static const std::string myClass;
  glm::mat4x4 transform;
  glm::mat4x4 orientation;
  int woot = 0;
  int farX, farY, farZ;
  const std::string* getClass() {
    return &myClass;
  }
  int getType() {
    return 2;
  }
  virtual void parentTransform();
  //virtual ~KObj_Oriented() {};
};

class KObj_Renderable : public KObj_Oriented {
 public:
  static const std::string myClass;
  //virtual int getType();
  const std::string* getClass() {
    return &myClass;
  }
  virtual void render()=0;
};

/** Used to represent actual objects in the game world through Components;
 *  type: 3
 */
class KObj_Entity : public KObj_Renderable {
 public:
  static const std::string myClass;
  glm::mat4x4 next;
  glm::vec3 velocity;
  glm::quat rotVelocity;

  std::vector<KComponent*> components;

  // 0: static
  // 1: something else
  uint8_t physics;

  // physics stuff
  glm::vec3 acceleration; // total calculated acceleration
  uint16_t radius; // radius of influence (farthest component)
  double accel;

  const std::string* getClass() {
    return &myClass;
  }
  int getType() {
    return 3;
  }
  void updateA();
  virtual void render();
  void parentTransform();
};

/** Refers to an entity, but still its own object; use this so
 * that entities do not have to be copied multiple times.
 *  type: 3
 */
class KObj_Instance : public KObj_Renderable {
 public:
  static const std::string myClass;
  const std::string* getClass() {
    return &myClass;
  }
  int getType() {
    return 4;
  }
  virtual void render() {
  }
};

// Component part
class KComponent {
 public:
  static const std::string myClass;
  bool collide;
  bool renderable;
  KObj_Entity* parent;  // this caused my first circular dependency
  void* jsObject;
  glm::mat4x4 offset;
  virtual const std::string* getClass() {
    return &myClass;
  }
  virtual void render() {

  }
  virtual void testCollision(KComponent& comp, Physics::CollisionInfo& ci) {

  }
  virtual ~KComponent() {

  }
 protected:
};

class KMaterial {
  //
  //
  //
  std::string source;
  GLuint vert;
  GLuint frag;
};

namespace KObj {

class GKO_World : public KObj_Node {
 public:
  static const std::string myClass;
  float timescale = 0;
  std::vector<uint16_t> world;
  std::vector<uint16_t> terrain;
  std::vector<uint16_t> forces;
  const std::string* getClass() {
    return &myClass;
  }
};

class OKO_Camera_ : public KObj_Oriented {
 public:
  static const std::string myClass;
  glm::vec3 center;
  glm::vec3 up;
  const std::string* getClass() {
    return &myClass;
  }
  void prespective();
  void parentTransform();
};

class OKO_Force : public KObj_Oriented {
 public:

  static const std::string myClass;

  // 0: typical acceleration
  // 1: constant velocity
  // 2: drag
  // 3: multiply
  uint8_t type;

  // 0: towards center
  // 1: towards orientation (OKO -z)
  // 2: towards association 0
  // 3: towards trajectory (speed up)
  uint8_t direction;

  // 0: global
  // 1: sphere/point
  // 100: associations collision
  uint8_t shape;

  // 0: linear
  // 1: inverse square
  // 2: constant
  uint8_t falloff;

  float strength;
  //float multiplier;

  // probably in meters
  float size;

  std::vector<uint16_t> associations;

  const std::string* getClass() {
    return &myClass;
  }
  void parentTransform();
};
}

namespace Physics {

class CollisionInfo {

 public:

  // if a collision happened
  bool collided;
  // how far A has been embedded into B
  float sink;
  // Time of contact.
  double collideTime;

  // Surface normals of collision
  glm::vec3 normA;
  glm::vec3 normB;

  // Where the collisions happened
  glm::vec3 spotA;
  glm::vec3 spotB;

  // Impact velocity from A-B relative to world, including rotations
  glm::vec3 impact;
  //double impactVelocity;
};

}

namespace Component {

class CPN_Cube : public KComponent {
 public:
  static const std::string myClass;
  const std::string* getClass() {
    return &myClass;
  }
  void render();
  void testCollision(KComponent& comp, Kondion::Physics::CollisionInfo& ci);
};

class CPN_InfinitePlane : public KComponent {
 public:
  static const std::string myClass;
  const std::string* getClass() {
    return &myClass;
  }
  void render();
  void testCollision(KComponent& comp, Kondion::Physics::CollisionInfo& ci);
};
}

double Delta();
std::string Dir();
void GameLoop();
void Launch();
void PhysicsUpdate();
double Time();

namespace Renderer {
extern KObj::OKO_Camera_* currentCamera;

class RenderPass {
 public:

  static const unsigned char DEFAULT = 0, FORWARD = 1, DIFFUSE = 2, DEPTH = 3,
      NORMALS = 4, LIGHT = 5, HDR = 6, GUI = 20;
  static std::vector<RenderPass*> passes;

  bool autoscan;
  bool ready = false;
  bool framebuffered = false;
  bool cameraOverride = false;
  std::vector<KObj_Renderable*> items;
  KObj::OKO_Camera_* camera;
  uint32_t drawLayer;
  uint16_t width, height;
  uint8_t type;

  void consider(KObj_Renderable* a);
  void force(KObj_Renderable* a);
  void generate();
  void render();
  void scan();
  GLuint id(uint8_t a) {
    return ids[a];
  }
  RenderPass(uint8_t typ, uint32_t layer, uint16_t w, uint16_t h, bool autoscn);

 protected:
  GLuint* ids;

};

GLuint CompileShader(GLenum type, const std::string& code,
                     const std::string& errorname);
void Composite();
void Consider(KObj_Renderable* a);
void Setup();
void Three(KObj::OKO_Camera_* c, uint16_t width, uint16_t height);
void Two(uint8_t window);
void RenderCube(float scale);
void RenderQuad(float width, float height);
}

namespace Window {

extern GLFWwindow* w;
int Initialize();
int CreateWindow(uint16_t width, uint16_t height);
int SwitchWindow(uint8_t index);
uint16_t GetWidth(uint8_t i);
uint16_t GetHeight(uint8_t i);
bool Active();
void Update();

}

namespace Input {
const uint8_t INPUT_SYSTEM = 0, INPUT_VIRTUAL = 1, INPUT_CONTROLLER = 10;

const uint16_t MOUSE_BUTTON = 400, MOUSE_POSX = 409, MOUSE_POSY = 410,
    MOUSE_INSIDE = 411, MOUSE_SCROLLX = 412, MOUSE_SCROLLY = 413,
    CONTROLLER_AXIS = 100, CONTROLLER_BUTTON = 0;

struct Control {

  static std::vector<Control*> controls;

  std::string name;

  uint8_t device;
  uint16_t button;
  //float x, y, px, py;
  float x, px;
  bool useDelta;
  bool alternate;
  bool invert;

  float delta() {
    return px - x;
  }

  Control(const std::string& name, uint8_t device, uint16_t button) {
    x = 0.0f;
    px = 0.0f;
    invert = false;
    alternate = false;
    useDelta = false;
    this->name = name;
    this->device = device;
    this->button = button;
  }
};

struct VirtualJoystick {

  static std::vector<VirtualJoystick*> vsticks;

  std::vector<uint16_t> controls; // there should be a better way of doing this
  std::vector<uint8_t> direction;
  std::vector<float> magnitude;

  std::string name;
  float x, px, y, py;
  bool clamp;

  void add(uint16_t index, uint8_t angle, float mag) {
    //if (Control::controls.size() <= index)
    //  return; // out of bounds
    controls.push_back(index);
    direction.push_back(angle);
    magnitude.push_back(mag);
  }

  VirtualJoystick(bool clamp, const std::string& prefix) {
    x = 0;
    y = 0;
    px = 0;
    py = 0;
    this->clamp = clamp;
    name = prefix;
    vsticks.push_back(this);
  }

};

uint16_t AddControl(const std::string& name, uint8_t device, uint16_t button);
void MouseLock(bool a);
Control* Get(uint16_t i);
uint16_t Count();
void Setup();
void Update();
uint16_t ControlIndex(const std::string& name);
float Value(uint16_t i);
float ValuePrev(uint16_t i);

void DebugPrint();
}

namespace Math {
void transform3f(const glm::mat4& a, const glm::vec3& b);

}

namespace Resources {

class KTexture {
 public:

  static std::vector<KTexture *> textures;
  static std::vector<uint16_t> loadMe;

  std::string source, identifier;
  uint16_t traits;
  uint16_t width, height;
  bool isInternal, isLoaded;//, mipmapped;
  //unsigned short imageWidth, imageHeight;
  //GLint minFilter, magFilter, textureId, wrapS, wrapT;
  GLuint textureId;

  //KTexture(std::string name, GLint id, uint16_t width, uint16_t height,
  //         GLint miFilter, GLint maFilter, GLint awrapS, GLint awrapT,
  //         bool mipped);
  KTexture(std::string name, GLint id, uint16_t width, uint16_t height);
  //KTexture(std::string name, std::string path, GLint miFilter, GLint maFilter,
  //         GLint awrapS, GLint awrapT);
  KTexture(std::string name, std::string path, uint16_t trait);
  void Load();
  void Bind();
};

class Raw {
 public:
  std::istream* stream;
  std::filebuf* fb;
  std::string filepath;
  uint8_t carton;
  Raw(const std::string& file) {
    fb = new std::filebuf;
    carton = 1; // CARTON_FOLDER
    filepath = file;
    fb->open(file.c_str(), std::ios::in);
    stream = new std::istream(fb);
  }
  ;
  ~Raw() {
    printf("raw deleted\n");
    fb->close();
    delete stream;
    delete fb;
  }
  ;
};

void AddCarton(const std::string& path);
void Load(const std::string& name, uint8_t type);
void Setup();
void Update();
Raw* Get(const std::string& url);

extern std::vector<KTexture *> textures;

}

namespace JS {

void CallFunction(const std::string& s);
void Destroy();
void Eval(const char* s);
void Eval(const std::string& s);
void GlobalUpdate();
void Setup();
void Start();
void UpdateInput();

namespace ON {
size_t Parse(const std::string& s);
size_t Parse(std::ifstream* s, const std::string& path);
std::string GetString(size_t id, const std::string& key);
void GetStringArray(size_t id, const std::string& key,
                    std::vector<std::string> &in);
size_t Enter(size_t id, const std::string& key);
void GetKeys(size_t id, std::vector<std::string> &in);
void Dispose(size_t id);
}

}

namespace Debug {
void printMatrix(const glm::mat4& a);
void printWorld();
}

}
#endif /* KONDION_H_ */
