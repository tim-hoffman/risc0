import "server-only";

import { tryit } from "radash";
import type { Version } from "~/types/version";

export async function fetchDatasheet({ version, url }: { version: Version; url: string }) {
  const tryFetch = tryit(fetch);
  const [error, response] = await tryFetch(
    `https://raw.githubusercontent.com/risc0/ghpages/${version}/dev/datasheet/${url}`,
    {
      next: { revalidate: 30, tags: ["fetch-datasheet"] }, // 30s cache
    },
  );

  // error handling
  if (error || !response.ok) {
    throw error || new Error("Failed to fetch");
  }

  return await response.json();
}
