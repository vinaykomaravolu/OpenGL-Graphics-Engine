#version 450 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

#define NUM_POINT_LIGHTS 48
#define NUM_DIR_LIGHTS 8
#define NUM_SPOT_LIGHTS 8

//Material
struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;

	bool enable;
};


//Light Structs
struct LightDir{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	bool enable;
};

struct LightPoint{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	bool enable;
};

struct LightSpot{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	bool enable;
};

//Material
uniform Material material;

//Lights
uniform LightDir lightDir[NUM_DIR_LIGHTS];
uniform LightPoint lightPoint[NUM_POINT_LIGHTS];
uniform LightSpot lightSpot[NUM_SPOT_LIGHTS];

//Texture sampler2D;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_height1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform bool texture_enable;


uniform vec3 viewPos;


//Function prototypes
vec3 CalcDirLight(LightDir light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(LightPoint light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(LightSpot light, vec3 normal, vec3 fragPos, vec3 viewDir);



void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = vec3(0.0f, 0.0f, 0.0f);
	for(int i = 0; i < NUM_DIR_LIGHTS; i++){
		result += CalcDirLight(lightDir[i], norm, viewDir);
	}
	for(int i = 0; i < NUM_POINT_LIGHTS; i++){
        result += CalcPointLight(lightPoint[i], norm, FragPos, viewDir);    
	}
	for(int i = 0; i < NUM_SPOT_LIGHTS; i++){
		result += CalcSpotLight(lightSpot[i],norm, FragPos, viewDir);
	}
	//ouput
	FragColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(LightDir light, vec3 normal, vec3 viewDir){
	if(light.enable == true){
		vec3 lightDir = normalize(-light.direction);
		float diff = max(dot(normal, lightDir), 0.0);

		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

		vec3 ambient  = light.ambient;
		vec3 diffuse  = light.diffuse * diff;
		vec3 specular = light.specular * spec;

		if(material.enable == true){
			ambient *= material.ambient;
			diffuse *= material.diffuse;
			specular *= material.diffuse;
		}
		if(texture_enable == true){
			ambient *= vec3(texture(texture_height1,TexCoords));
			diffuse *= vec3(texture(texture_diffuse1,TexCoords));
			specular *= vec3(texture(texture_specular1,TexCoords));
		}
		return (ambient + diffuse + specular);
	}
	return vec3(0.0f);
}

vec3 CalcPointLight(LightPoint light, vec3 normal, vec3 fragPos, vec3 viewDir){
	if(light.enable == true){
		vec3 lightDir = normalize(light.position - fragPos);
		float diff = max(dot(normal, lightDir),0.0);

		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

		float distance    = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
   
		vec3 ambient  = light.ambient;
		vec3 diffuse  = light.diffuse * diff;
		vec3 specular = light.specular * spec;
		if(material.enable == true){
			ambient *= material.ambient;
			diffuse *= material.diffuse;
			specular *= material.diffuse;
		}
		if(texture_enable == true){
			ambient *= vec3(texture(texture_height1,TexCoords));
			diffuse *= vec3(texture(texture_diffuse1,TexCoords));
			specular *= vec3(texture(texture_specular1,TexCoords));
		}
		ambient  *= attenuation;
		diffuse  *= attenuation;
		specular *= attenuation;

		return (ambient + diffuse + specular);
   }
   return vec3(0.0f);
}

vec3 CalcSpotLight(LightSpot light, vec3 normal, vec3 fragPos, vec3 viewDir){
	if(light.enable == true){
		vec3 lightDir = normalize(light.position - fragPos);
		float theta = dot(lightDir, normalize(-light.direction)); 

        float diff = max(dot(normal, lightDir), 0.0);

		vec3 reflectDir = reflect(-lightDir, normal);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

		float epsilon = (light.cutOff - light.outerCutOff);
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

		float distance    = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
   
		vec3 ambient  = light.ambient;
		vec3 diffuse  = light.diffuse * diff;
		vec3 specular = light.specular * spec;
		if(material.enable == true){
			ambient *= material.ambient;
			diffuse *= material.diffuse;
			specular *= material.diffuse;
		}
		if(texture_enable == true){
			ambient *= vec3(texture(texture_height1,TexCoords));
			diffuse *= vec3(texture(texture_diffuse1,TexCoords));
			specular *= vec3(texture(texture_specular1,TexCoords));
		}
		ambient  *= attenuation; 
		diffuse  *= attenuation;
	    specular *= attenuation;
		diffuse  *= intensity;
		specular *= intensity;
		return (ambient + diffuse + specular);
	}
	return vec3(0.0f);
}