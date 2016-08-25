# Contributing to vector-tile

## Releasing

To release a new vector-tile version:

 - Make sure all tests are passing on travis
 - Update version number in
   - include/mapbox/vector-tile/version.hpp
   - update unit tests of version constants in test/unit/vector_tile.test.cpp
 - Update CHANGELOG.md
 - Update UPGRADING.md
 - Create a new tag and push to github `git push --tags`
 - Publish to npm:
   - First run `make testpack` to see what files will be published
   - If you see any unwanted files, then add them to `.npmignore`
   - Then run: `npm publish`

