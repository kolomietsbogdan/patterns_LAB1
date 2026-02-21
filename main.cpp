#include <QCoreApplication>
#include <iostream>
#include <vector>
#include <cstddef>

// ====================== 1. Prototype ======================
class GraphObject {
protected:
    bool isColored;

public:
    GraphObject(bool colored = true) : isColored(colored) {}
    virtual ~GraphObject() = default;

    virtual GraphObject* clone() const = 0;
    virtual void draw() const = 0;
    virtual size_t memorySize() const = 0;
};

class Point : public GraphObject {
    double x, y;
public:
    Point(double x = 0, double y = 0, bool colored = true)
        : GraphObject(colored), x(x), y(y) {}

    GraphObject* clone() const override { return new Point(*this); }

    void draw() const override {
        std::cout << (isColored ? "Color" : "B/W") << " Point (" << x << ", " << y << ")\n";
    }
    size_t memorySize() const override { return sizeof(Point); }
};

class Line : public GraphObject {
    double x1, y1, x2, y2;
public:
    Line(double x1=0, double y1=0, double x2=0, double y2=0, bool colored=true)
        : GraphObject(colored), x1(x1), y1(y1), x2(x2), y2(y2) {}

    GraphObject* clone() const override { return new Line(*this); }

    void draw() const override {
        std::cout << (isColored ? "Color" : "B/W") << " Line (" << x1 << "," << y1
                  << ")-(" << x2 << "," << y2 << ")\n";
    }
    size_t memorySize() const override { return sizeof(Line); }
};

class Circle : public GraphObject {
    double cx, cy, r;
public:
    Circle(double cx=0, double cy=0, double r=1, bool colored=true)
        : GraphObject(colored), cx(cx), cy(cy), r(r) {}

    GraphObject* clone() const override { return new Circle(*this); }

    void draw() const override {
        std::cout << (isColored ? "Color" : "B/W") << " Circle (" << cx << "," << cy << ") r=" << r << "\n";
    }
    size_t memorySize() const override { return sizeof(Circle); }
};

// ====================== 2. Singleton ======================
class Scene {
private:
    static Scene* instance;
    std::vector<GraphObject*> objects;

    Scene() = default;

public:
    static Scene* getInstance() {
        if (!instance) instance = new Scene();
        return instance;
    }

    void addObject(GraphObject* obj) {
        if (obj) objects.push_back(obj);
    }

    void drawAll() const {
        std::cout << "=== What the scene contains ===\n";
        for (const auto* obj : objects)
            obj->draw();
    }

    ~Scene() {
        for (auto* obj : objects) delete obj;
    }
};

Scene* Scene::instance = nullptr;

// ====================== 3. Abstract Factory ======================
class AbstractGraphFactory {
public:
    virtual ~AbstractGraphFactory() = default;
    virtual GraphObject* createPoint(double x = 0, double y = 0) = 0;
    virtual GraphObject* createLine(double x1=0, double y1=0, double x2=0, double y2=0) = 0;
    virtual GraphObject* createCircle(double cx=0, double cy=0, double r=1) = 0;
};

class ColorGraphFactory : public AbstractGraphFactory {
public:
    GraphObject* createPoint(double x = 0, double y = 0) override {
        GraphObject* p = new Point(x, y, true);
        Scene::getInstance()->addObject(p); // hook for auto additing
        return p;
    }
    GraphObject* createLine(double x1=0, double y1=0, double x2=0, double y2=0) override {
        GraphObject* l = new Line(x1, y1, x2, y2, true);
        Scene::getInstance()->addObject(l); // hook for auto additing
        return l;
    }
    GraphObject* createCircle(double cx=0, double cy=0, double r=1) override {
        GraphObject* c = new Circle(cx, cy, r, true);
        Scene::getInstance()->addObject(c); // hook for auto additing
        return c;
    }
};

class BWGraphFactory : public AbstractGraphFactory {
public:
    GraphObject* createPoint(double x = 0, double y = 0) override {
        GraphObject* p = new Point(x, y, false);
        Scene::getInstance()->addObject(p); // hook for auto additing
        return p;
    }
    GraphObject* createLine(double x1=0, double y1=0, double x2=0, double y2=0) override {
        GraphObject* l = new Line(x1, y1, x2, y2, false);
        Scene::getInstance()->addObject(l); // hook for auto additing
        return l;
    }
    GraphObject* createCircle(double cx=0, double cy=0, double r=1) override {
        GraphObject* c = new Circle(cx, cy, r, false);
        Scene::getInstance()->addObject(c); // hook for auto additing
        return c;
    }
};

// ====================== 4. Builders ======================
// Scene Builder
class SceneBuilder {
public:
    virtual ~SceneBuilder() = default;
    virtual void buildPoint(double x=0, double y=0) = 0;
    virtual void buildLine(double x1=0,double y1=0,double x2=0,double y2=0) = 0;
    virtual void buildCircle(double cx=0,double cy=0,double r=1) = 0;
    virtual void getResult() = 0;
};

// Real Scene Builder
class RealSceneBuilder : public SceneBuilder {
    AbstractGraphFactory* factory;
public:
    RealSceneBuilder(AbstractGraphFactory* f) : factory(f) {}
    void buildPoint(double x=0, double y=0) override { factory->createPoint(x, y); }
    void buildLine(double x1=0,double y1=0,double x2=0,double y2=0) override { factory->createLine(x1,y1,x2,y2); }
    void buildCircle(double cx=0,double cy=0,double r=1) override { factory->createCircle(cx,cy,r); }
    void getResult() override { Scene::getInstance()->drawAll(); }
};

// Memory comput Builder
class MemorySceneBuilder : public SceneBuilder {
    size_t mem = 0;
public:
    void buildPoint(double, double) override { mem += sizeof(Point); }
    void buildLine(double, double, double, double) override { mem += sizeof(Line); }
    void buildCircle(double, double, double) override { mem += sizeof(Circle); }

    void getResult() override {
        std::cout << "Total memory to use is: " << mem << " bytes\n";
    }
};

// Director
class SceneDirector {
public:
    void constructTestScene(SceneBuilder* builder) {
        builder->buildPoint(10, 20);
        builder->buildLine(0, 0, 100, 50);
        builder->buildCircle(50, 50, 25);
    }
};

// ====================== main ======================
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "=== Colored scene (Abstract Factory + Real Builder + Singleton + Hook) ===\n";
    ColorGraphFactory colorFactory;
    RealSceneBuilder realBuilder(&colorFactory);
    SceneDirector director;
    director.constructTestScene(&realBuilder);
    realBuilder.getResult();

    std::cout << "\n=== Memory count (Memory Builder) ===\n";
    MemorySceneBuilder memoryBuilder;
    director.constructTestScene(&memoryBuilder);
    memoryBuilder.getResult();

    // Prototype demo
    std::cout << "\n=== Prototype demonstration ===\n";
    Point original(5, 5, true);
    GraphObject* copy = original.clone();
    copy->draw();
    delete copy;

    return a.exec();
}
