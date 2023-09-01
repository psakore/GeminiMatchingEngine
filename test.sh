#!/bin/bash
sudo docker build --build-arg TESTING=ON --build-arg CACHEBUST=$(date +%s) -t gemini_matching_engine . 
#&& docker run -i --rm gemini_matching_engine /app/MatchingEngine/install/bin/matching_engine_test
