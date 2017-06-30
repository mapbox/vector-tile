
# VTZero TODO

## Questions

* How strict do we want to be? For instance regarding geometry?
  Current code contains a "strict" flag that enables some extra checks.
  See for instance commented-out check in `feature::decode_polygon_geometry`.

## Optimization

* `layer_builder` code contains `std::map<string, uint32_t>`. Should use
  `map` or `unordered_map` of `<data_view, uint32_t>`. Maybe `m_keys_map` can
  even simply be a vector we do brute force search on. It is unlikely
  there will be that many keys in a layer.
* Option to pre-set key/value tables (when copying layers or when they
  are known anyway)?

## Other

* Change interfaces for feature and layer iteration. Use something similar to
  the tags iterator?
* `feature::decode_*_geometry` functions might be better as free functions.
* Add functions to convert tag value to a `variant`?
* Add functions to convert mercator coordinates to tile coordinates?

