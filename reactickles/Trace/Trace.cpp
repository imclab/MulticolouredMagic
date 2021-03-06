/**
 * == Simplified BSD *** MODIFIED FOR NON-COMMERCIAL USE ONLY!!! *** ==
 * Copyright (c) 2011, Cariad Interactive LTD
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 *   * Redistributions of source code must retain the above copyright notice, this list of 
 *     conditions and the following disclaimer.
 * 
 *   * Redistributions in binary form must reproduce the above copyright notice, this list of 
 *     conditions and the following disclaimer in the documentation and/or other materials provided 
 *     with the distribution.
 * 
 *   * Any redistribution, use, or modification is done solely for personal benefit and not for any 
 *     commercial purpose or for monetary gain
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 *  Trace.cpp
 *
 *  Created by Marek Bereza on 27/10/2011.
 */

#include "Trace.h"
/*
 *  Trace.h
 *
 *
 */

#include "Reactickle.h"
#include "TraceShape.h"
#include "ofxOsc.h"
#include "ReactickleApp.h"


void Trace::setup() {
	maximumLengthOfTrace = 200; //start with 200, make this interactive?
	positionInTrace = 0;
	
	currShapeID = MAGIC_CIRCLE;
}

void Trace::update() {
	for(int i=0; i < traces.size(); i++){
		traces[i].update();
	}
}

void Trace::draw() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	for(int i=0; i < traces.size(); i++){
		traces[i].draw();
	}
}

bool Trace::touchDown(float x, float y, int touchId){
	
	touchMoved(x, y, touchId);

	
	return true;
}

bool Trace::touchMoved(float x, float y, int touchId){
	addNewTraceToTraces(x,y); 
	if(mode==2) {
		float dx = x - WIDTH/2;
		float dy = y - HEIGHT/2;
		float dist = sqrt(dx*dx + dy*dy);
		float angle = atan2(dy, dx);
		angle -= PI;
		x = dist*cos(angle) + WIDTH/2;
		y = dist*sin(angle) + HEIGHT/2;
		addNewTraceToTraces(x, y);
	}
	
	return true;
}

bool Trace::touchUp(float x, float y, int touchId){
	return true;
} 

void Trace::addNewTraceToTraces(float x, float y){
	
	if(mode==2) {
		currShapeID = ofGetElapsedTimef();
		currShapeID %= NUM_MAGIC_SHAPES;
	}
	else if(mode == 1){
		currShapeID++;
		currShapeID %= NUM_MAGIC_SHAPES;
		
		/*if (currShapeID == MAGIC_CIRCLE){
		 currShapeID = MAGIC_CROSS;
		 }else{
		 currShapeID = MAGIC_CIRCLE;
		 }*/
		
	}else{
		currShapeID = MAGIC_CIRCLE;
	}    
	
#ifndef TARGET_OF_IPHONE
	ofxOscMessage m;
	m.setAddress( "/shapechange" );
	m.addIntArg( currShapeID );
	ReactickleApp::instance->sender.sendMessage( m );
#endif
	
	
	if(traces.size() < maximumLengthOfTrace){
		//then we want to keep growing the vector....
		TraceShape newShape;
		newShape.setup(ofVec2f(x,y), currShapeID);
		traces.push_back(newShape);
	}else{
		//otherwise, change an "old" trace shape
		traces[positionInTrace].setup(ofVec2f(x,y), currShapeID);
	}
	
	positionInTrace++;
	
	positionInTrace %= maximumLengthOfTrace;
}
