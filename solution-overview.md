# Solution Overview

## Core Mechanism
Solar Monitor reads a PPM image and divides it into a grid of regions. For every region it calculates an anomaly score based on how different the local pixel intensity is from the image-wide average.

The region with the strongest score is selected as the candidate fault region. The program then:
1. Finds the center of the candidate region.
2. Estimates a radius from the region size.
3. Draws a red circle around it.
4. Saves the annotated image.
5. Writes a report containing the location and score.

## Why this approach
The prototype avoids OpenCV and external dependencies. This makes compilation easier on a fresh machine and keeps the core algorithm visible to evaluators.

## User Experience
The operator provides an input image and receives:
- an annotated image with a red circle;
- a text report containing the detected region;
- an anomaly score that can be used for prioritization.

## Future Improvement
A production version can replace the heuristic detector with a trained thermal/visual anomaly-detection model and can add confidence calibration, multiple fault regions, historical monitoring, and IBM Bob/watsonx conversational analysis.
