void main()
{
	vec3 normal = normalize(
		axesInfoMatrix * (255/128 * texture2D(normalMap, texCoord0.xy).xyz) - 1);
    gl_FragColor = texture2D(diffuse, texCoord0.xy) * 
    	CalcLightingEffect(normalize(normal0), worldPos0);
}
