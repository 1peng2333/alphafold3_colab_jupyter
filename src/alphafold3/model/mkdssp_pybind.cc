// Copyright 2024 DeepMind Technologies Limited
//
// AlphaFold 3 source code is licensed under CC BY-NC-SA 4.0. To view a copy of
// this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/
//
// To request access to the AlphaFold 3 model parameters, follow the process set
// out at https://github.com/google-deepmind/alphafold3. You may only use these
// if received directly from Google. Use is subject to terms of use available at
// https://github.com/google-deepmind/alphafold3/blob/main/WEIGHTS_TERMS_OF_USE.md

#include "alphafold3/model/mkdssp_pybind.h"

#include <stdlib.h>
#include <string>

#include <cif++/file.hpp>
#include <cif++/pdb.hpp>
#include <dssp.hpp>
#include <sstream>

#include "absl/strings/string_view.h"
#include "pybind11/pybind11.h"

namespace alphafold3 {
namespace py = pybind11;

void RegisterModuleMkdssp(pybind11::module m) {
  py::module resources = py::module::import("importlib.resources");
  py::module share_libcifpp = py::module::import("share.libcifpp");
  std::string data_dir =
      py::cast<std::string>(resources.attr("files")(share_libcifpp)
                                .attr("joinpath")('.')
                                .attr("as_posix")());
  setenv("LIBCIFPP_DATA_DIR", data_dir.c_str(), 0);
  m.def(
      "get_dssp",
      [](absl::string_view mmcif, int model_no,
         int min_poly_proline_stretch_length,
         bool calculate_surface_accessibility) {
        cif::file cif_file(mmcif.data(), mmcif.size());
        dssp result(cif_file.front(), model_no, min_poly_proline_stretch_length,
                    calculate_surface_accessibility);
        std::stringstream sstream;
        result.write_legacy_output(sstream);
        return sstream.str();
      },
      py::arg("mmcif"), py::arg("model_no") = 1,
      py::arg("min_poly_proline_stretch_length") = 3,
      py::arg("calculate_surface_accessibility") = false,
      py::doc("Gets secondary structure from an mmCIF file."));
}

}  // namespace alphafold3
