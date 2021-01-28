#pragma once

//TODO: dodać globalne zmienne ze skyboxa, ex.: extern <typ> globalVar;


void initializeSkybox();
void renderSkybox();
void deleteSkybox();

unsigned int loadTexture(char const * path)
unsigned int loadCubemap(vector<std::string> faces);
