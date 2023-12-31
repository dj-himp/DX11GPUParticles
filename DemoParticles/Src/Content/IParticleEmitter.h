#pragma once

using json = nlohmann::json;

namespace DemoParticles
{
    class Camera;

    enum EmitterType
    {
        ET_Point,
        ET_Sphere,
        ET_Cube,
        ET_Buffer,
        ET_Mesh,
        ET_AppendBuffer,

        ET_Count
    };

    class IParticleEmitter
    {
    public:
        

        IParticleEmitter(const DX::DeviceResources* deviceResources, std::string name, EmitterType type);
        virtual ~IParticleEmitter();

        virtual void createDeviceDependentResources() = 0;

        virtual void update(DX::StepTimer const& timer) = 0;
        virtual void emit() = 0;
        virtual void RenderImGui(Camera* camera) { (void)camera; }
        virtual void reset() {}
        virtual void save(json& file) { (void)file; }
        virtual void load(json& file) { (void)file; }
        virtual std::string toString() = 0;

        std::string getName() { return m_name; }
    protected:
        const DX::DeviceResources* m_deviceResources;

        std::string m_name;
        bool m_enabled = true;
        EmitterType m_type;
        int m_guizmoUniqueID;

    private:
        static inline int guizmoUniqueIDGenerator = 0;
    };
}
