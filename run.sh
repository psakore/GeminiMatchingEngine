#!/bin/bash
sudo docker build --build-arg TESTING=OFF -t gemini_matching_engine . && docker run -i --rm gemini_matching_engine /app/MatchingEngine/install/bin/matching_engine
