# Problem Statement

## Who is affected?
Solar-panel owners, maintenance teams, technicians, and operators need to identify abnormal regions on panels so that inspection and maintenance can be prioritized.

## What is the problem?
A solar panel can contain regions that differ significantly from the surrounding panel because of damage, hotspots, cracks, dirt, or other abnormalities. Finding these regions manually can require repeated visual inspection.

## Why it matters
An inspection system should help an operator quickly answer:
- Where is the suspicious region?
- How strong is the detected anomaly?
- Which image should be inspected more closely?

## Proposed direction
Solar Monitor focuses on a simple first-stage detector: analyze the image, find the most suspicious region, and provide an interpretable visual marker. The prototype is deliberately lightweight so it can be built and tested using a standard C++ compiler.
