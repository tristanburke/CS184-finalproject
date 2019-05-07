#version 330

layout(location = 0) out vec4 glFragColor;

//! <group name="Camera Properties"/>
uniform vec3 CameraPosition = vec3(0.0, 0.0, 0.0); //!slider[(-100,-100,-100), (0, 0,0), (100,100,100)]

//! <group name="Object Properties"/>
uniform float SpecularPower = 300.0; //! slider[10, 300, 1000]
uniform vec3 Ka = vec3(0.7); //! color[0.7, 0.7, 0.7]
uniform vec3 Kd = vec3(0.9, 0.8, 1.0); //! color[(0.9, 0.8, 1.0]
uniform vec3 Ks = vec3(0.7); //! color[0.7, 0.7, 0.7]
uniform vec3 Ke = vec3(0.7); //! color[0.7, 0.7, 0.7]
uniform vec3 baseColor = vec3(0.7); //! color[0.7, 0.7, 0.7]

//! <group name="Light Properties"/>
uniform vec3 LightPosition = vec3(0.0, 0.0, 0.0); //!slider[(-100,-100,-100), (0, 0,0), (100,100,100)]
uniform vec3 Ia = vec3 (0.3); //! color[0.3, 0.3, 0.3]
uniform vec3 Id = vec3 (1.0); //! color[1.0, 1.0, 1.0]
uniform vec3 Is = vec3 (0.7); //! color[0.7, 0.7, 0.7]

//! <group name="Border Properties"/>
uniform vec3 borderColor= vec3(0.0); //! color[0.0, 0.0, 0.0]
uniform float borderTolerance = 0.0001; //! slider[0.0001, 0.5, 0.9999]

//! <group name="Color and Specular Steps"/>
uniform int colorSteps; //! slider[1, 5, 10]
uniform int specularSteps; //! slider[1, 5, 10]


//! <group name="Usage Properties"/>
uniform bool manualCamPos = false;  //! checkbox[false]
uniform bool shading = false;  //! checkbox[true]
uniform bool usePhongBlinn = true; //! checkbox[true]
uniform bool seeOutLine = true; //! checkbox[true]
uniform bool seeColorSteps = false; //! checkbox[false]
uniform bool seeSpecularSteps = true; //! checkbox[true]

// From Vertex shader
in vec3 vPosition;
in vec3 vNormal;


vec3 N;
vec3 V;
vec3 L;
float LN;
float CamOBJNorm;

// Methods
// Phong Shade declaration
vec3 shadePhong();
// Phong-Blinn Shade declaration
vec3 shadePhongBlinn();

vec4 getFinalColor();

void main()
{
    glFragColor = getFinalColor();
}

vec4 getFinalColor()
{
    if (manualCamPos)
    {
        V = normalize(CameraPosition-vPosition);
    }
    else
    {
        V = normalize(-vPosition);
    }
    
    
    N = normalize (vNormal);
    L = normalize(LightPosition - vPosition);
    CamOBJNorm = dot(N,V);
    LN = dot(L,N);
    
    if (seeColorSteps)
    {
        LN = ceil(LN * colorSteps) / colorSteps;
    }
    
    if (seeOutLine && CamOBJNorm <= borderTolerance)
    {
        return vec4(borderColor, 1.0);
    }
    else if (shading)
    {
        if (usePhongBlinn)
        {
            return vec4(shadePhongBlinn(), 1.0);
        }
        else
        {
            return vec4(shadePhong(), 1.0);
        }
    }
    
    return vec4(baseColor, 1.0);
    
}

vec3 shadePhong()
{
    vec3 c = vec3(0.0);
    
    // Ambiental: Ka * Ambient Color (Ia)
    c = Ia * Ka;
    
    // Diffuse: Kd * LightColorDiffuse (Id) * max(0, dot(N,L))
    vec3 diffuse = Id * Kd * max(0.0, LN);
    c += clamp(diffuse, 0.0, 1.0);
    
    // Specular: Ks * LightColorSpecular (Is) * pow(max (0, dot(R,V)), SpecularPower)
    vec3 R = reflect(-L, N);
    float RV = dot (R,V);
    
    if (seeSpecularSteps)
    {
        RV = ceil(RV * specularSteps) / specularSteps;
    }
    
    float factor = max (RV, 0.01); // en apuntes: max (0.0, RV);
    vec3 specular = Is*Ks*pow(factor, SpecularPower);
    
    c += clamp(specular, 0.0, 1.0);
    
    // Emissive: Ke
    c+=Ke;
    
    return c;
}

vec3 shadePhongBlinn()
{
    vec3 c = vec3(0.0);
    
    // Ambiental: Ka * Ambient Color (Ia)
    c = Ia * Ka;
    
    // Diffuse: Kd * LightColorDiffuse (Id) * max(0, dot(N,L))
    vec3 diffuse = Id * Kd * max(0.0, LN);
    c += clamp(diffuse, 0.0, 1.0);
    
    // Phong-Blinn changes specular
    // Specular: Ks * LightColorSpecular (Is) * pow(max (0, dot(R,V)), SpecularPower)
    vec3 halfAngleVector = normalize(L + V);
    float HAVN= dot (N, halfAngleVector);
    
    if (seeSpecularSteps)
    {
        HAVN = ceil(HAVN * specularSteps) / specularSteps;
    }
    
    float factor = max (HAVN, 0.01);
    vec3 specular = Is*Ks*pow(factor, SpecularPower);
    c += clamp(specular, 0.0, 1.0);
    
    // Emissive: Ke
    c+=Ke;
    
    return c;
}
//! <preset file="toonShading.preset" />
